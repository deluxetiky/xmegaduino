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

int analogRead(uint8_t pin)
{
#if 0
	uint8_t low, high;

	// set the analog reference (high two bits of ADMUX) and select the
	// channel (low 4 bits).  this also sets ADLAR (left-adjust result)
	// to 0 (the default).
	ADMUX = (analog_reference << 6) | (pin & 0x07);

	// without a delay, we seem to read from the wrong channel
	delay(1);

	// start the conversion
	sbi(ADCSRA, ADSC);

	// ADSC is cleared when the conversion finishes
	while (bit_is_set(ADCSRA, ADSC));

	// we have to read ADCL first; doing so locks both ADCL
	// and ADCH until ADCH is read.  reading ADCL second would
	// cause the results of each conversion to be discarded,
	// as ADCL and ADCH would be locked when it completed.
	low = ADCL;
	high = ADCH;

	// combine the two bytes
	return (high << 8) | low;
#else
        ADC_t* adc;
        if ( pin < 8 ) {
            adc = &ADCA;
        } else if ( pin < 16 ) {
            adc = &ADCB;
            pin -= 8;
        } else {
            return -1;
        }

        adc->CTRLA   = ADC_DMASEL_OFF_gc // DMA off
                     | 0                 // don't start ADC
                     | 0                 // don't flush
                     | ADC_ENABLE_bm     // enable
                     ;

        // TODO: Signed conversions?
        adc->CTRLB   = ADC_CONMODE_bm           // signed conversion
                     | 0                        // no freerun
                     | ADC_RESOLUTION_12BIT_gc  // 12bit resolution
                     ;

        // TODO: What's the correct analog ref?
        // TODO: What the heck is bandgap
        adc->REFCTRL = ADC_REFSEL_VCC_gc // VCC/1.6 analog ref
                     | 0                 // bandgap enable
                     | 0                 // temerature reference enable
                     ;

        adc->EVCTRL = ADC_SWEEP_0_gc    // Have to set it to something, but no sweep.
                    | ADC_EVSEL_7_gc    // Have to set it to something, but no sweep.
                    | ADC_EVACT_NONE_gc // No event action
                    ;

        // TODO: What's correct for prescalar mode?
        adc->PRESCALER = ADC_PRESCALER_DIV512_gc;

        adc->INTFLAGS = 0; // No interrupt on conversion complete

        adc->CALL = 0; // TODO: Need to set

        adc->CALH = 0; // TODO: Need to set

        adc->CH0.MUXCTRL = 1 << (ADC_CH_MUXPOS_gp + pin); // Select pin for positive input

        // TODO: What shoult INTCTRL be?
        adc->CH0.INTCTRL = 0;
	
        adc->CH0.INTFLAGS = 0xF; // Strangely enough, clears IF
	
        // TODO: What's correct for input mode?
        adc->CH0.CTRL = ADC_CH_START_bm                 // Start conversion
                      | ADC_CH_GAIN_1X_gc               // 1x gain
                      | ADC_CH_INPUTMODE_SINGLEENDED_gc // single ended
                      ;

        while (!adc->INTFLAGS); // wait for adc to finish
        adc->INTFLAGS = 0xF;

        uint16_t result;
        result = (adc->CH0.RESH << 8) + adc->CH0.RESL;
        // With a signed mode conversion x<2048 means the high bit is clear, which
        // means the result is positive, otherwise it is negative.
        // We want to normalize  the results between [0 to 4096).
#if 0
        if ( result < 2048 ) {
            // Positive results need to be normalized to [2048, 4096), which
            // is easily done by adding 2048.
            result += 2048;
        } else if ( result < 4096 ) {
            // Negative results need to be normalized to [0-4096).
            // Due to the properties of two's complement the below does this.
            result = 2047 - result + 1;
        } else {
            // Something insane has happened xmegas aren't supposed to do this ....
        }
#endif

	return result;
#endif
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
	
	/*
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
	 */
}
