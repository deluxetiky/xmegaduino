/*
  HardwareSerial.cpp - Hardware serial library for Wiring
  Copyright (c) 2006 Nicholas Zambetti.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
  
  Modified 23 November 2006 by David A. Mellis
*/

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "wiring.h"
#include "wiring_private.h"

#include "HardwareSerial.h"

// Define constants and variables for buffering incoming serial data.  We're
// using a ring buffer (I think), in which rx_buffer_head is the index of the
// location to which to write the next incoming character and rx_buffer_tail
// is the index of the location from which to read.
#define RX_BUFFER_SIZE 128

struct ring_buffer {
  unsigned char buffer[RX_BUFFER_SIZE];
  int head;
  int tail;
};

ring_buffer rx_buffer_c0 = { { 0 }, 0, 0 };
ring_buffer rx_buffer_d0 = { { 0 }, 0, 0 };
ring_buffer rx_buffer_d1 = { { 0 }, 0, 0 };

inline void store_char(unsigned char c, ring_buffer *rx_buffer)
{
  int i = (rx_buffer->head + 1) % RX_BUFFER_SIZE;

  // if we should be storing the received character into the location
  // just before the tail (meaning that the head would advance to the
  // current location of the tail), we're about to overflow the buffer
  // and so we don't write the character or advance the head.
  if (i != rx_buffer->tail) {
    rx_buffer->buffer[rx_buffer->head] = c;
    rx_buffer->head = i;
  }
}


#if 1
//TODO: gc: ISR or SIGNAL?
ISR(USARTC0_RXC_vect)
{
  unsigned char c = USARTC0.DATA;
  store_char(c, &rx_buffer_c0);
}

ISR(USARTD0_RXC_vect)
{
  unsigned char c = USARTD0.DATA;
  store_char(c, &rx_buffer_d0);
}

ISR(USARTD1_RXC_vect)
{
  unsigned char c = USARTD1.DATA;
  store_char(c, &rx_buffer_d1);
}

#else

SIGNAL(USART_RX_vect)
{
  unsigned char c = UDR0;
  store_char(c, &rx_buffer);
}

#endif


// Constructors ////////////////////////////////////////////////////////////////

#if 1
HardwareSerial::HardwareSerial(
  ring_buffer *rx_buffer,
  USART_t     *usart,
  PORT_t      *port,
  uint8_t      in_bm,
  uint8_t      out_bm,
  uint8_t      u2x)
{
  this->_rx_buffer = rx_buffer;
  this->_usart     = usart;
  this->_port      = port;
  this->_in_bm     = in_bm;
  this->_out_bm    = out_bm;
  this->_u2x       = u2x;
}
#else
HardwareSerial::HardwareSerial(ring_buffer *rx_buffer,
  volatile uint8_t *ubrrh, volatile uint8_t *ubrrl,
  volatile uint8_t *ucsra, volatile uint8_t *ucsrb,
  volatile uint8_t *udr,
  uint8_t rxen, uint8_t txen, uint8_t rxcie, uint8_t udre, uint8_t u2x)
{
  _rx_buffer = rx_buffer;
  _ubrrh = ubrrh;
  _ubrrl = ubrrl;
  _ucsra = ucsra;
  _ucsrb = ucsrb;
  _udr = udr;
  _rxen = rxen;
  _txen = txen;
  _rxcie = rxcie;
  _udre = udre;
  _u2x = u2x;
}
#endif

// Public Methods //////////////////////////////////////////////////////////////

void HardwareSerial::bsel(unsigned value)
{
  _usart->BAUDCTRLA = value;
  _usart->BAUDCTRLB = value >> 8;
}

void HardwareSerial::begin(long baud)
{
  uint16_t baud_setting;
  bool use_u2x;

#if 1
  // TODO: Serial. Fix serial double clock.
  use_u2x = false;

  _port->DIRCLR = _in_bm;  // input
  _port->DIRSET = _out_bm; // output

  // set the baud rate
  if (use_u2x) {
    _usart->CTRLB |= 1 << _u2x;
    baud_setting = F_CPU / 8 / baud - 1;
  } else {
    baud_setting = F_CPU / 16 / baud - 1;
  }
  _usart->BAUDCTRLA = (uint8_t)baud_setting;
  _usart->BAUDCTRLB = baud_setting >> 8;


  // enable Rx and Tx
  _usart->CTRLB |= USART_RXEN_bm | USART_TXEN_bm;
  // enable interrupt
  _usart->CTRLA = (_usart->CTRLA & ~USART_RXCINTLVL_gm) | USART_RXCINTLVL_LO_gc;

  // Char size, parity and stop bits: 8N1
  _usart->CTRLC = USART_CHSIZE_8BIT_gc | USART_PMODE_DISABLED_gc;

  // Set PMIC to level low
  PMIC.CTRL |= PMIC_LOLVLEX_bm;

  // Enable global interrupts
  sei();

#else
  // U2X mode is needed for baud rates higher than (CPU Hz / 16)
  if (baud > F_CPU / 16) {
    use_u2x = true;
  } else {
    // figure out if U2X mode would allow for a better connection
    
    // calculate the percent difference between the baud-rate specified and
    // the real baud rate for both U2X and non-U2X mode (0-255 error percent)
    uint8_t nonu2x_baud_error = abs((int)(255-((F_CPU/(16*(((F_CPU/8/baud-1)/2)+1))*255)/baud)));
    uint8_t u2x_baud_error    = abs((int)(255-((F_CPU/( 8*(((F_CPU/4/baud-1)/2)+1))*255)/baud)));
    
    // prefer non-U2X mode because it handles clock skew better
    use_u2x = (nonu2x_baud_error > u2x_baud_error);
  }
  
  if (use_u2x) {
    *_ucsra = 1 << _u2x;
    baud_setting = (F_CPU / 4 / baud - 1) / 2;
  } else {
    *_ucsra = 0;
    baud_setting = (F_CPU / 8 / baud - 1) / 2;
  }

  // assign the baud_setting, a.k.a. ubbr (USART Baud Rate Register)
  *_ubrrh = baud_setting >> 8;
  *_ubrrl = baud_setting;

  sbi(*_ucsrb, _rxcie);
  sbi(*_ucsrb, _rxen);
  cbi(*_ucsrb, _txen);
#endif
}

void HardwareSerial::end()
{
#if 1
  // disable Rx and Tx
  _usart->CTRLB &= ~(USART_RXEN_bm | USART_TXEN_bm);
  // disable interrupt
  _usart->CTRLA = (_usart->CTRLA & ~USART_RXCINTLVL_gm) | USART_RXCINTLVL_LO_gc;
#else
  cbi(*_ucsrb, _rxen);
  cbi(*_ucsrb, _txen);
  cbi(*_ucsrb, _rxcie);  
#endif
}

uint8_t HardwareSerial::available(void)
{
  return (RX_BUFFER_SIZE + _rx_buffer->head - _rx_buffer->tail) % RX_BUFFER_SIZE;
}

int HardwareSerial::read(void)
{
  // if the head isn't ahead of the tail, we don't have any characters
  if (_rx_buffer->head == _rx_buffer->tail) {
    return -1;
  } else {
    unsigned char c = _rx_buffer->buffer[_rx_buffer->tail];
    _rx_buffer->tail = (_rx_buffer->tail + 1) % RX_BUFFER_SIZE;
    return c;
  }
}

void HardwareSerial::flush()
{
  // don't reverse this or there may be problems if the RX interrupt
  // occurs after reading the value of rx_buffer_head but before writing
  // the value to rx_buffer_tail; the previous value of rx_buffer_head
  // may be written to rx_buffer_tail, making it appear as if the buffer
  // were full, not empty.
  _rx_buffer->head = _rx_buffer->tail;
}

void HardwareSerial::write(uint8_t c)
{
  while ( !(_usart->STATUS & USART_DREIF_bm) );
  _usart->DATA = c;
}

// Preinstantiate Objects //////////////////////////////////////////////////////
#if 1
    HardwareSerial Serial (&rx_buffer_c0, &USARTC0, &PORTC, PIN2_bm, PIN3_bm, USART_CLK2X_bp);
    HardwareSerial Serial1(&rx_buffer_d0, &USARTD0, &PORTD, PIN2_bm, PIN3_bm, USART_CLK2X_bp);
    HardwareSerial Serial2(&rx_buffer_d1, &USARTD1, &PORTD, PIN6_bm, PIN7_bm, USART_CLK2X_bp);

    extern HardwareSerial* Serial1ptr = &Serial1;
#else
    extern HardwareSerial Serial(&rx_buffer, &UBRR0H, &UBRR0L, &UCSR0A, &UCSR0B, &UDR0, RXEN0, TXEN0, RXCIE0, UDRE0, U2X0);
#endif

#if 1

extern "C" void diag_ln() {
    Serial1.println();
}

extern "C" void diag(const char* str) {
    Serial1.print(str);
}

extern "C" void diagln(const char* str) {
    diag(str);
    diag_ln();
}

extern "C" void diagN(long n) {
    Serial1.print(n);
}

extern "C" void diagN2(long n, int base) {
    Serial1.print(n,base);
}

#endif
