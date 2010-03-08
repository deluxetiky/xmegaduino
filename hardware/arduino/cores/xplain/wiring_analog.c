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

// TODO: Wrap all this frufra within objects
const TC0_t* PROGMEM timer_to_tc0_PGM[] = {
    NULL,

    &TCD0,
    &TCD0,
    &TCD0,
    &TCD0,
    NULL,
    NULL,

    &TCE0,
    &TCE0,
    &TCE0,
    &TCE0,
    NULL,
    NULL,
};

const TC0_t* PROGMEM timer_to_tc1_PGM[] = {
    NULL,

    NULL,
    NULL,
    NULL,
    NULL,
    &TCD1,
    &TCD1,

    NULL,
    NULL,
    NULL,
    NULL,
    &TCE1,
    &TCE1,
};

const uint8_t PROGMEM timer_to_channel_PGM[] = {
    NULL,

    0,
    1,
    2,
    3,
    0,
    1,

    0,
    1,
    2,
    3,
    0,
    1,
};


// TODO: templatize for TC1_t*
static void InitializeTC0(TC0_t* tc)
{
    // 


    // TODO: Should we use channel D (for TC0 and B for TC1)?
    // Enable Compare channel A.
    tc->CCA =TC0_CCAEN_bm;

    // TODO: 2, 4, 8, 64, 256 or 1024?
    // Clock source.
    tc->CTRLA = ( tc->CTRLA & ~TC0_CLKSEL_gm ) | TC_CLKSEL_DIV1024_gc;
}

// TODO: Add pwm12, pwm16, and pwm24.

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

	pinMode(pin, OUTPUT);

        uint8_t timer_index   = pgm_read_byte(digital_pin_to_timer_PGM + pin);
        TC0_t*  tc0           = pgm_read_word(timer_to_tc0_PGM         + timer_index);
        TC1_t*  tc1           = pgm_read_word(timer_to_tc1_PGM         + timer_index);
        uint8_t channel_index = pgm_read_byte(timer_to_channel_PGM     + timer_index);


        // freq pwm = 32Mhz / (2 x N x PER)
        /// TODO: Can we run at 31Khz, well above the audible range?
        /// Issue is whether existing circuits can handle this.
        /// TODO: Add API to set freq pwm. Default to 1000khz,
        /// compatible with existing code.
        /// N = 32Mhz / (2 x freq pwm x PER)
        // 976hz = 32Mhz / (2 x 64 x 0xFF)
        if ( tc0 ) {
            tc0->PERBUF = 0xFF;
            tc0->CTRLA  = TC_CLKSEL_DIV64_gc;

            /// TODO: Update pin at waveform top, bottom, or both?
            /// Seems that by doing at top and bottom we can increase the
            /// pwm freq and resolution.
            /// For now, do at bottom for compatibility.

            // Dual slope mode.
            tc0->CTRLB  = ( tc0->CTRLB & ~TC0_WGMODE_gm ) | TC_WGMODE_DS_B_gc; // TODO: Factor out and move to wiring.c init.
            tc0->CTRLB |= TC0_CCAEN_bm << channel_index;
        } else if ( tc1 ) {
            tc1->PERBUF  = 0xFF;
            tc1->CTRLA   = TC_CLKSEL_DIV64_gc;
            tc1->CTRLB   = ( tc1->CTRLB & ~TC1_WGMODE_gm ) | TC_WGMODE_DS_B_gc;
            tc1->CTRLB  |= TC1_CCAEN_bm << channel_index;
	} else if (val < 128) {
            digitalWrite(pin, LOW);
            return;
	} else {
            digitalWrite(pin, HIGH);
            return;
        }

        // TODO: Add timer_to_channel.
        // Set value
        if ( tc0 && 0 == channel_index ) {
            tc0->CCABUF = val;
        } else if ( tc0 && 1 == channel_index ) {
            tc0->CCBBUF = val;
        } else if ( tc0 && 2 == channel_index ) {
            tc0->CCCBUF = val;
        } else if ( tc0 && 3 == channel_index ) {
            tc0->CCDBUF = val;
        } else if ( tc1 && 0 == channel_index ) {
            tc1->CCABUF = val;
        } else if ( tc1 && 1 == channel_index ) {
            tc1->CCBBUF = val;
	} else if (val < 128) {
           digitalWrite(pin, LOW);
	} else {
            digitalWrite(pin, HIGH);
        }
}
