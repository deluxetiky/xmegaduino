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

// Constructors ////////////////////////////////////////////////////////////////

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

// Public Methods //////////////////////////////////////////////////////////////

void HardwareSerial::begin(long baud)
{
  uint16_t baud_setting;
  uint8_t bscale = 0;
  bool use_u2x;

  // TODO: Serial. Fix serial double clock.
  use_u2x = false;

  _port->DIRCLR = _in_bm;  // input
  _port->DIRSET = _out_bm; // output

  // Reset flags
  _usart->CTRLB = 0;
  
  // set the baud rate
#if (F_CPU == 32000000L)
  // Known good settings calculated from ProtoTalk Calc
  // Baud rates less than 38400 can use the default algorithm
  if (baud == 38400) {
	  baud_setting = 3269;
	  bscale = 10;
  }
  else if(baud == 57600) {
	  baud_setting = 2158;
	  bscale = 10;
  }
  else if(baud == 115200) {
	  baud_setting = 1047;
	  bscale = 10;
  }
  else
#endif
  if (use_u2x) {
    _usart->CTRLB |= 1 << _u2x;
    baud_setting = ((F_CPU) / ((uint32_t)baud * 8) - 1);
  } else {
    baud_setting = ((F_CPU) / ((uint32_t)baud * 16) - 1);
  }
  _usart->BAUDCTRLA = (uint8_t)baud_setting;
  _usart->BAUDCTRLB = (bscale << 4) | (baud_setting >> 8);


  // enable Rx and Tx
  _usart->CTRLB |= USART_RXEN_bm | USART_TXEN_bm;
  // enable interrupt
  _usart->CTRLA = (_usart->CTRLA & ~USART_RXCINTLVL_gm) | USART_RXCINTLVL_LO_gc;

  // Char size, parity and stop bits: 8N1
  _usart->CTRLC = USART_CHSIZE_8BIT_gc | USART_PMODE_DISABLED_gc;
}

void HardwareSerial::end()
{
  // disable Rx and Tx
  _usart->CTRLB &= ~(USART_RXEN_bm | USART_TXEN_bm);
  // disable interrupt
  _usart->CTRLA = (_usart->CTRLA & ~USART_RXCINTLVL_gm) | USART_RXCINTLVL_LO_gc;
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

#define SERIAL_DEFINE(name, usart, port, read_bm, write_bm) \
ring_buffer name##rx_buffer = { { 0 }, 0, 0 }; \
ISR(usart##_RXC_vect) \
{ \
  unsigned char c = usart.DATA; \
  store_char(c, &name##rx_buffer); \
} \
HardwareSerial name (&name##rx_buffer, &usart, &port, read_bm, write_bm, USART_CLK2X_bp);

#define SERIAL0_USART    USARTC0
#define SERIAL0_PORT     PORTC
#define SERIAL0_READ_BM  PIN2_bm
#define SERIAL0_WRITE_BM PIN3_bm
#if defined SERIAL0_USART
    SERIAL_DEFINE(Serial, SERIAL0_USART, SERIAL0_PORT, SERIAL0_READ_BM, SERIAL0_WRITE_BM);
#endif
#if 0 // TODO: Kill this
ring_buffer rx_buffer_c0 = { { 0 }, 0, 0 };
ISR(USARTC0_RXC_vect)
{
  unsigned char c = USARTC0.DATA;
  store_char(c, &rx_buffer_c0);
}
HardwareSerial Serial (&rx_buffer_c0, &USARTC0, &PORTC, PIN2_bm, PIN3_bm, USART_CLK2X_bp);
#endif

ring_buffer rx_buffer_d0 = { { 0 }, 0, 0 };
ISR(USARTD0_RXC_vect)
{
  unsigned char c = USARTD0.DATA;
  store_char(c, &rx_buffer_d0);
}
HardwareSerial Serial1(&rx_buffer_d0, &USARTD0, &PORTD, PIN2_bm, PIN3_bm, USART_CLK2X_bp);

ring_buffer rx_buffer_d1 = { { 0 }, 0, 0 };
ISR(USARTD1_RXC_vect)
{
  unsigned char c = USARTD1.DATA;
  store_char(c, &rx_buffer_d1);
}
HardwareSerial Serial2(&rx_buffer_d1, &USARTD1, &PORTD, PIN6_bm, PIN7_bm, USART_CLK2X_bp);

#if 1
// TODO: Move to diag.{c h}

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
