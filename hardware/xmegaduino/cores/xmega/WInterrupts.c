/* -*- mode: jde; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Part of the Wiring project - http://wiring.uniandes.edu.co

  Copyright (c) 2004-05 Hernando Barragan

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General
  Public License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA  02111-1307  USA
  
  Modified 24 November 2006 by David A. Mellis
*/

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdio.h>

#include "pins_arduino.h"
#include "WConstants.h"
#include "wiring_private.h"

volatile static voidFuncPtr intFunc[EXTERNAL_NUM_INTERRUPTS];
static uint8_t portLastValue[EXTERNAL_NUM_INTERRUPTS/8];
// volatile static voidFuncPtr twiIntFunc;

#if defined(__AVR_ATmega8__)
#define EICRA MCUCR
#define EIMSK GICR
#endif

/// TODO: Add param to userFunc
void attachInterrupt(uint8_t pin, void (*userFunc)(void), int mode) {
  if( EXTERNAL_NUM_INTERRUPTS <= pin ) {
      return;
  }
  intFunc[pin] = userFunc;
    
  // Configure the interrupt mode (trigger on low input, any change, rising
  // edge, or falling edge).  The mode constants were chosen to correspond
  // to the configuration bits in the hardware register, so we simply shift
  // the mode into place.
      
  // Enable the interrupt.

  uint8_t  portIndex = digitalPinToPort(pin);
  PORT_t*  port      = portRegister(portIndex);
  uint8_t* pinctrl   = &port->PIN0CTRL;

  port->INTCTRL  |= PORT_INT1LVL_LO_gc;
  port->INT1MASK |= 1 << (pin&7);
  pinctrl[pin]   |= mode;
  portLastValue[portIndex] = port->IN;
}

void detachInterrupt(uint8_t pin) {
  if( EXTERNAL_NUM_INTERRUPTS <= pin ) {
      return;
  }
      
  uint8_t  portIndex = digitalPinToPort(pin);
  PORT_t*  port      = portRegister(portIndex);
  uint8_t* pinctrl   = &port->PIN0CTRL;

  intFunc[pin] = 0;
  port->INT1MASK &= ~(1 << (pin&7));
  if ( 0 == port->INT1MASK ) {
    // No interrupts on any of ports pins, turn it off.
    port->INTCTRL &= ~PORT_INT1LVL_gm;
  }
  pinctrl[pin] &= ~PORT_ISC_gm;
}

/*
void attachInterruptTwi(void (*userFunc)(void) ) {
  twiIntFunc = userFunc;
}
*/

void PORT_INT( int portIndex )
{
  PORT_t* port = portRegister(portIndex);

  uint8_t inValue = port->IN;
  uint8_t pin     = (portIndex-1)*8;
  int     index;
  for ( index = 0; index < 8; ++index ) {
    if ( inValue & 1 ) {
      intFunc[pin]();
    }
    inValue >>= 1;
    ++pin;
  }

  port->INTFLAGS &= ~PORT_INT1IF_bm;
}

ISR(PORTA_INT1_vect)
{
  PORT_INT(PA);
}

ISR(PORTB_INT1_vect)
{
  PORT_INT(PB);
}

ISR(PORTC_INT1_vect)
{
  PORT_INT(PC);
}

ISR(PORTD_INT1_vect)
{
  PORT_INT(PD);
}

ISR(PORTE_INT1_vect)
{
  PORT_INT(PE);
}

ISR(PORTF_INT1_vect)
{
  PORT_INT(PF);
}

