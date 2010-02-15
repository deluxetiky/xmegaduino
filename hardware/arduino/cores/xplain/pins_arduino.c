/*
  pins_arduino.c - pin definitions for the Arduino board
  Part of Arduino / Wiring Lite

  Copyright (c) 2005 David A. Mellis

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

  $Id: pins_arduino.c 804 2009-12-18 16:05:52Z dmellis $
*/

#include <avr/io.h>
#include "wiring_private.h"
#include "pins_arduino.h"

// First draft of xplain pin mapping
// 07.Feb.2010 by R. Bohne
//
// Alternate xplain pin mapping
// 2010-02-14 GorillaCoder
//
// (PWM+ indicates the additional PWM pins on the ATmega168.)


#define PA 1
#define PB 2
#define PC 3
#define PD 4
#define PE 5
#define PF 6

#define REPEAT8(x) x, x, x, x, x, x, x, x
#define BV0TO7 _BV(0), _BV(1), _BV(2), _BV(3), _BV(4), _BV(5), _BV(6), _BV(7)
#define BV7TO0 _BV(7), _BV(6), _BV(5), _BV(4), _BV(3), _BV(2), _BV(1), _BV(0)


#define DDRA PORTA_DIR
#define DDRB PORTB_DIR
#define DDRC PORTC_DIR
#define DDRD PORTD_DIR
#define DDRE PORTE_DIR
#define DDRF PORTF_DIR

#define PORTA PORTA_OUT
#define PORTB PORTB_OUT
#define PORTC PORTC_OUT
#define PORTD PORTD_OUT
#define PORTE PORTE_OUT
#define PORTF PORTF_OUT

#define PINA PORTA_IN
#define PINB PORTB_IN
#define PINC PORTC_IN
#define PIND PORTD_IN
#define PINE PORTE_IN
#define PINF PORTF_IN



const uint16_t PROGMEM port_to_mode_PGM[] = {
	NOT_A_PORT,
	&DDRA,//pin header, analog in
	&DDRB,//internal, analog in, pot and speaker
	&DDRC,//internal, usartc0 to usb on 2&3
	&DDRD,//pin header
	&DDRE,//leds
	&DDRF,//switches
};

const uint16_t PROGMEM port_to_output_PGM[] = {
	NOT_A_PORT,
	&PORTA,//pin header, analog in
	&PORTB,//internal, analog in, pot and speaker
	&PORTC,//internal, usartc0 to usb on 2&3
	&PORTD,//pin header
	&PORTE,//leds
	&PORTF,//switches
};

const uint16_t PROGMEM port_to_input_PGM[] = {
	NOT_A_PIN,
	&PINA,//pin header, analog in
	&PINB,//internal, analog in, pot and speaker
	&PINC,//internal, usartc0 to usb on 2&3
	&PIND,//pin header
	&PINE,//leds
	&PINF,//switches
};

#define PIN_MAP_RENE 1
#define PIN_MAP_GIULIANO 2
#define PIN_MAP PIN_MAP_GIULIANO

#ifndef PIN_MAP
#define PIN_MAP PIN_MAP_DEFAULT
#endif

#if PIN_MAP_RENE == PIN_MAP
const uint8_t PROGMEM digital_pin_to_port_PGM[] = {
	// PORTLIST
	PC,
	PC,
	PA,
	PA,
	PA,
	PD,
	PD,
	PA,
	PA,
	PA,
	PD,
	PD,
	PF,//PD, //DEBUG !!!
	PE,//PD, //DEBUG !!!
	PD,
	PB,
	PB,
	PB,
	PB,
	PD,
	PD,
};

const uint8_t PROGMEM digital_pin_to_bit_mask_PGM[] = {
	// PIN IN PORT		
	// -------------------------------------------		
	_BV( 2 )	,
	_BV( 3 )	,	
	_BV( 0 )	,
	_BV( 2 )	,	
	_BV( 1 )	,
	_BV( 2 )	,
	_BV( 3 )	,	
	_BV( 5 )	,	
	_BV( 6 )	,
	_BV( 7 )	,	
	_BV( 4 )	,
	_BV( 5 )	,
	_BV( 6 )	,	
	_BV( 7 )	,
	_BV( 4 )	,	
	_BV( 5 )	,
	_BV( 6 )	,
	_BV( 7 )	,	
	_BV( 0 )	,	
	_BV( 1 )	,
	};

const uint8_t PROGMEM digital_pin_to_timer_PGM[] = {
	// TIMERS		
	// -------------------------------------------		
	NOT_ON_TIMER	, 
	NOT_ON_TIMER	, 
	NOT_ON_TIMER	, 
	NOT_ON_TIMER	, 
	NOT_ON_TIMER	, 
	NOT_ON_TIMER	, 
	NOT_ON_TIMER	, 
	NOT_ON_TIMER	, 
	NOT_ON_TIMER	, 
	NOT_ON_TIMER	, 
	NOT_ON_TIMER	, 
	NOT_ON_TIMER	, 
	NOT_ON_TIMER	, 
	NOT_ON_TIMER	, 
	NOT_ON_TIMER	, 
	NOT_ON_TIMER	, 
	NOT_ON_TIMER	, 
	NOT_ON_TIMER	, 
	NOT_ON_TIMER	, 
	NOT_ON_TIMER	, 
	};
#elif PIN_MAP_GIULIANO == PIN_MAP
const uint8_t PROGMEM digital_pin_to_port_PGM[] = {
	// PORTLIST
        REPEAT8(PC), // USARTC0 connected to USB on 2&3
        REPEAT8(PD), // Header
        REPEAT8(PE), // LEDs
        REPEAT8(PF), // Switches
        REPEAT8(PA), // Header
        REPEAT8(PB), // Pot and Speaker
};

const uint8_t PROGMEM digital_pin_to_bit_mask_PGM[] = {
	// PIN IN PORT		
	// -------------------------------------------		
	BV0TO7, // PORT C USARTC0 to USB on 2&3
	BV0TO7, // PORT D Header
	BV0TO7, // PORT E LEDs
	BV0TO7, // PORT F Switches
	BV0TO7, // PORT A Header
	BV0TO7, // PORT B Pot and Speaker
	};

const uint8_t PROGMEM digital_pin_to_timer_PGM[] = {
	// TIMERS		
	// -------------------------------------------		
	REPEAT8(NOT_ON_TIMER), // PORT C USARTC0 to USB on 2&3
	REPEAT8(NOT_ON_TIMER), // PORT D Header
	REPEAT8(NOT_ON_TIMER), // PORT E LEDs
	REPEAT8(NOT_ON_TIMER), // PORT F Switches
	REPEAT8(NOT_ON_TIMER), // PORT A Header
	REPEAT8(NOT_ON_TIMER), // PORT B Pot and Speaker
	};

#else

#error No pin map defined

#endif
