/*
  wiring_analog.c - analog input and output
  Part of Arduino - http://www.arduino.cc/

  Copyright (c) 2005-2006 David A. Mellis

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

  $Id: wiring.c 248 2007-02-03 15:36:30Z mellis $
*/

#include "wiring_private.h"
#include "pins_arduino.h"

uint8_t analog_reference = DEFAULT;

void analogReference(uint8_t mode)
{
	// can't actually set the register here because the default setting
	// will connect AVCC and the AREF pin, which would cause a short if
	// there's something connected to AREF.
	analog_reference = mode;
}

int analogRead12(uint8_t pin)
{
        ADC_t* adc;
        if ( pin < 8 ) {
            adc = &ADCA;
        } else if ( pin < 16 ) {
            adc = &ADCB;
            pin -= 8;
        } else {
            return -1;
        }

        adc->CH0.MUXCTRL = pin << ADC_CH_MUXPOS_gp; // Select pin for positive input

        adc->CH0.CTRL |= ADC_CH_START_bm; // Start conversion
        while ( 0 == (adc->CH0.INTFLAGS & ADC_CH_CHIF_bm) ); // wait for adc to finish
        adc->CH0.INTFLAGS = 1;

        uint16_t result = adc->CH0RES;

	return result;
}

int analogRead(uint8_t pin)
{
    return analogRead12(pin) >> 2; // 10 bits
}

// 24 bits let's users represent up to 256 volts using a fixed point representation.
long analogRead24(uint8_t pin)
{
    return ( (long)analogRead12(pin) ) << 12;
}

unsigned analogRead16(uint8_t pin)
{
    return ( (unsigned)analogRead12(pin) ) << 4;
}

// Right now, PWM output only works on the pins with
// hardware support.  These are defined in the appropriate
// pins_*.c file.  For the rest of the pins, we default
// to digital output.
void analogWrite(uint8_t pin, int val)
{
	// We need to make sure the PWM output is enabled for those pins
	// that support it, as we turn it off when digitally reading or
	// writing with them.  Also, make sure the pin is in output mode
	// for consistenty with Wiring, which doesn't require a pinMode
	// call for the analog output pins.
	
#if 0
	pinMode(pin, OUTPUT);
	
	if (digitalPinToTimer(pin) == TIMER1A) {
		// connect pwm to pin on timer 1, channel A
		sbi(TCCR1A, COM1A1);
		// set pwm duty
		OCR1A = val;
	} else if (digitalPinToTimer(pin) == TIMER1B) {
		// connect pwm to pin on timer 1, channel B
		sbi(TCCR1A, COM1B1);
		// set pwm duty
		OCR1B = val;
	} else if (digitalPinToTimer(pin) == TIMER0A) {
		if (val == 0) {
			digitalWrite(pin, LOW);
		} else {
			// connect pwm to pin on timer 0, channel A
			sbi(TCCR0A, COM0A1);
			// set pwm duty
			OCR0A = val;      
		}
	} else if (digitalPinToTimer(pin) == TIMER0B) {
		if (val == 0) {
			digitalWrite(pin, LOW);
		} else {
			// connect pwm to pin on timer 0, channel B
			sbi(TCCR0A, COM0B1);
			// set pwm duty
			OCR0B = val;
		}
	} else if (digitalPinToTimer(pin) == TIMER2A) {
		// connect pwm to pin on timer 2, channel A
		sbi(TCCR2A, COM2A1);
		// set pwm duty
		OCR2A = val;	
	} else if (digitalPinToTimer(pin) == TIMER2B) {
		// connect pwm to pin on timer 2, channel B
		sbi(TCCR2A, COM2B1);
		// set pwm duty
		OCR2B = val;
	} else if (val < 128)
		digitalWrite(pin, LOW);
	else
		digitalWrite(pin, HIGH);
#endif
}
