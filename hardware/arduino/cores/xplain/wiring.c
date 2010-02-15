/*
  wiring.c - Partial implementation of the Wiring API for the ATmega8.
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

  $Id: wiring.c 808 2009-12-18 17:44:08Z dmellis $
*/

#include "wiring_private.h"

volatile unsigned long rtc_millis = 0;

/*! \brief RTC overflow interrupt service routine.
 *
 *  This ISR keeps track of the milliseconds 
 */
ISR(RTC_OVF_vect)
{
	rtc_millis = rtc_millis+4;
}


unsigned long millis()
{
	
	unsigned long m;
	
	uint8_t oldSREG = SREG;

	// disable interrupts while we read rtc_millis or we might get an
	// inconsistent value (e.g. in the middle of a write to rtc_millis)
	cli();
	m = rtc_millis;
	SREG = oldSREG;

	return m;
}

unsigned long micros() {
	/*
	unsigned long m;
	
	uint8_t oldSREG = SREG, t;
	
	cli();
	m = timer0_overflow_count;
	t = TCNT0;
  
#ifdef TIFR0
	if ((TIFR0 & _BV(TOV0)) && (t < 255))
		m++;
#else
	if ((TIFR & _BV(TOV0)) && (t < 255))
		m++;
#endif

	SREG = oldSREG;
	
	return ((m << 8) + t) * (64 / clockCyclesPerMicrosecond());
	 */
	return 0;
}

void delay(unsigned long ms)
{
	
	unsigned long start = millis();
	
	while (millis() - start <= ms)
		;
}

/* Delay for the given number of microseconds.  Assumes a 8 or 16 MHz clock. */
void delayMicroseconds(unsigned int us)
{
	// calling avrlib's delay_us() function with low values (e.g. 1 or
	// 2 microseconds) gives delays longer than desired.
	//delay_us(us);

#if F_CPU >= 16000000L
	// for the 16 MHz clock on most Arduino boards

	// for a one-microsecond delay, simply return.  the overhead
	// of the function call yields a delay of approximately 1 1/8 us.
	if (--us == 0)
		return;

	// the following loop takes a quarter of a microsecond (4 cycles)
	// per iteration, so execute it four times for each microsecond of
	// delay requested.
	us <<= 2;

	// account for the time taken in the preceeding commands.
	us -= 2;
#else
	// for the 8 MHz internal clock on the ATmega168

	// for a one- or two-microsecond delay, simply return.  the overhead of
	// the function calls takes more than two microseconds.  can't just
	// subtract two, since us is unsigned; we'd overflow.
	if (--us == 0)
		return;
	if (--us == 0)
		return;

	// the following loop takes half of a microsecond (4 cycles)
	// per iteration, so execute it twice for each microsecond of
	// delay requested.
	us <<= 1;
    
	// partially compensate for the time taken by the preceeding commands.
	// we can't subtract any more than this or we'd overflow w/ small delays.
	us--;
#endif

	// busy wait
	__asm__ __volatile__ (
		"1: sbiw %0,1" "\n\t" // 2 cycles
		"brne 1b" : "=w" (us) : "0" (us) // 2 cycles
	);
}

void init()
{
	// this needs to be called before setup() or some functions won't
	// work there
	sei();
	
	// on the ATmega168, timer 0 is also used for fast hardware pwm
	// (using phase-correct PWM would mean that timer 0 overflowed half as often
	// resulting in different millis() behavior on the ATmega8 and ATmega168)
	//sbi(TCCR0A, WGM01);
	//sbi(TCCR0A, WGM00);

	// set timer 0 prescale factor to 64
	//sbi(TCCR0B, CS01);
	//sbi(TCCR0B, CS00);

	// enable timer 0 overflow interrupt
	//sbi(TIMSK0, TOIE0);


	// timers 1 and 2 are used for phase-correct hardware pwm
	// this is better for motors as it ensures an even waveform
	// note, however, that fast pwm mode can achieve a frequency of up
	// 8 MHz (with a 16 MHz clock) at 50% duty cycle

	// set timer 1 prescale factor to 64
	//sbi(TCCR1B, CS11);
	//sbi(TCCR1B, CS10);
	// put timer 1 in 8-bit phase correct pwm mode
	//sbi(TCCR1A, WGM10);

	// set timer 2 prescale factor to 64
	//sbi(TCCR2B, CS22);

	// configure timer 2 for phase correct pwm (8-bit)
	//sbi(TCCR2A, WGM20);

	// set a2d prescale factor to 128
	// 16 MHz / 128 = 125 KHz, inside the desired 50-200 KHz range.
	// XXX: this will not work properly for other clock speeds, and
	// this code should use F_CPU to determine the prescale factor.
	//sbi(ADCSRA, ADPS2);
	//sbi(ADCSRA, ADPS1);
	//sbi(ADCSRA, ADPS0);

	// enable a2d conversions
	//sbi(ADCSRA, ADEN);

	// the bootloader connects pins 0 and 1 to the USART; disconnect them
	// here so they can be used as normal digital i/o; they will be
	// reconnected in Serial.begin()
	//UCSR0B = 0;
	
        /**************************************/
        /* Init system clock: run it at 32Mhz */

        // Enable 32M internal crystal
        OSC.CTRL |= OSC_RC32MEN_bm;
        // Wait for it to stablize
        while ( !(OSC.STATUS & OSC_RC32MEN_bm) ) ;

#if 0
        /*  Probably won't work, but would be neat if we could run 32Mhz
            clock through 4x PLL and have it run at 128Mhz. Datasheet implies
            this should work. But everything I read says 32Mhz max.
        */
        // Does this go before or after internal 32M internal crystal in enabled and stable?
        // Dunno, try after first.
    
        // PLL source: internal 32M crystal multiplied by 4 - max PLL factor for 32M crystal.
        OSC.PLLCTRL = ((uint8_t)OSC_PLLSRC_RC2M_gc) | ( (4&OSC_PLLFAC_gm) << OSC_PLLFAC_gp);
        // Enable PLL
        OSC.CTRL |= OSC_PLLEN_bm;
        // Wait for PLL to stabilize
        while ( !(OSC.STATUS & OSC_PLLRDY_bm ) ) ;
#endif

        // #define CLOCK_SOURCE CLK_SCLKSEL_PLL_gc
        #define CLOCK_SOURCE CLK_SCLKSEL_RC32M_gc

        register uint8_t value = (CLK.CTRL & ~CLK_SCLKSEL_gm ) | CLOCK_SOURCE;

        // Set main system clock to PLL
        asm volatile(
            "ldi  r30, lo8(%0)     \n\t"
            "ldi  r31, hi8(%0)     \n\t"
            "ldi  r16,  %2         \n\t"
            "out   %3, r16         \n\t"
            "st     Z,  %1         \n\t"
            :
            : "i" (&CLK.CTRL),
              "r" ( value ),
              "M" (CCP_IOREG_gc),
              "i" (&CCP)
            : "r16", "r30", "r31"
        );

        /*************************************/
        /* Init real time clock for millis() */
	
	/* Turn on internal 32kHz. */
	OSC.CTRL |= OSC_RC32KEN_bm;

	do {
		/* Wait for the 32kHz oscillator to stabilize. */
	} while ( ( OSC.STATUS & OSC_RC32KRDY_bm ) == 0);
		

	/* Set internal 32kHz oscillator as clock source for RTC. */
	CLK.RTCCTRL = CLK_RTCSRC_RCOSC_gc | CLK_RTCEN_bm;//1kHz

#define TOTEMPOLE 0x00  // Totempole
#define BUSKEEPER 0x08  // Buskeeper
#define WIRED_AND 0x38  // Wired-AND-PullUp
#define OUT_PULL_CONFIG BUSKEEPER
	
	//configure pins of xmega
	PORTCFG.MPCMASK = 0xFF; //do this for all pins of the following command
	PORTA.PIN0CTRL = OUT_PULL_CONFIG;

	PORTCFG.MPCMASK = 0xFF; //do this for all pins of the following command
	PORTB.PIN0CTRL = OUT_PULL_CONFIG;
	
	PORTCFG.MPCMASK = 0xFF; //do this for all pins of the following command
	PORTD.PIN0CTRL = OUT_PULL_CONFIG;

	PORTCFG.MPCMASK = 0xFF; //do this for all pins of the following command
	PORTE.PIN0CTRL = OUT_PULL_CONFIG;

	PORTCFG.MPCMASK = 0xFF; //do this for all pins of the following command
	PORTF.PIN0CTRL = OUT_PULL_CONFIG;
	
	do {
		/* Wait until RTC is not busy. */
	} while (  RTC.STATUS & RTC_SYNCBUSY_bm );
	
	/* Configure RTC period to 1 millisecond. */
	RTC.PER = 0;//1ms
	RTC.CNT = 0;
	RTC.COMP = 0;
	RTC.CTRL = ( RTC.CTRL & ~RTC_PRESCALER_gm ) | RTC_PRESCALER_DIV1_gc;

	/* Enable overflow interrupt. */	
	RTC.INTCTRL = ( RTC.INTCTRL & ~( RTC_COMPINTLVL_gm | RTC_OVFINTLVL_gm ) ) |
	              RTC_OVFINTLVL_LO_gc |
	              RTC_COMPINTLVL_OFF_gc;

	/* Enable interrupts. */
	PMIC.CTRL |= PMIC_LOLVLEN_bm;
	sei();
	
	
		
	
}
