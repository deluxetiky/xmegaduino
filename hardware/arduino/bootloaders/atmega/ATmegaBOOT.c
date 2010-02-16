/**********************************************************/
/* Serial Bootloader for Atmel megaAVR Controllers        */
/*                                                        */
/* tested with ATmega8, ATmega128 and ATmega168           */
/* should work with other mega's, see code for details    */
/*                                                        */
/* ATmegaBOOT.c                                           */
/*                                                        */
/*                                                        */
/* 20090308: integrated Mega changes into main bootloader */
/*           source by D. Mellis                          */
/* 20080930: hacked for Arduino Mega (with the 1280       */
/*           processor, backwards compatible)             */
/*           by D. Cuartielles                            */
/* 20070626: hacked for Arduino Diecimila (which auto-    */
/*           resets when a USB connection is made to it)  */
/*           by D. Mellis                                 */
/* 20060802: hacked for Arduino by D. Cuartielles         */
/*           based on a previous hack by D. Mellis        */
/*           and D. Cuartielles                           */
/*                                                        */
/* Monitor and debug functions were added to the original */
/* code by Dr. Erik Lins, chip45.com. (See below)         */
/*                                                        */
/* Thanks to Karl Pitrich for fixing a bootloader pin     */
/* problem and more informative LED blinking!             */
/*                                                        */
/* For the latest version see:                            */
/* http://www.chip45.com/                                 */
/*                                                        */
/* ------------------------------------------------------ */
/*                                                        */
/* based on stk500boot.c                                  */
/* Copyright (c) 2003, Jason P. Kyle                      */
/* All rights reserved.                                   */
/* see avr1.org for original file and information         */
/*                                                        */
/* This program is free software; you can redistribute it */
/* and/or modify it under the terms of the GNU General    */
/* Public License as published by the Free Software       */
/* Foundation; either version 2 of the License, or        */
/* (at your option) any later version.                    */
/*                                                        */
/* This program is distributed in the hope that it will   */
/* be useful, but WITHOUT ANY WARRANTY; without even the  */
/* implied warranty of MERCHANTABILITY or FITNESS FOR A   */
/* PARTICULAR PURPOSE.  See the GNU General Public        */
/* License for more details.                              */
/*                                                        */
/* You should have received a copy of the GNU General     */
/* Public License along with this program; if not, write  */
/* to the Free Software Foundation, Inc.,                 */
/* 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA */
/*                                                        */
/* Licence can be viewed at                               */
/* http://www.fsf.org/licenses/gpl.txt                    */
/*                                                        */
/* Target = Atmel AVR m128,m64,m32,m16,m8,m162,m163,m169, */
/* m8515,m8535. ATmega161 has a very small boot block so  */
/* isn't supported.                                       */
/*                                                        */
/* Tested with m168                                       */
/**********************************************************/

/* $Id$ */


/* some includes */
#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include "config.h"

/* function prototypes */
void putch(char);
char getch(void);
void getNch(uint8_t);
void byte_response(uint8_t);
void nothing_response(void);
char gethex(void);
void puthex(char);
void flash_led(uint8_t);
void HandleChar(int c);
void LoadProgram();

static inline void    CheckWatchDogAtStartup();
static inline void    SetBootloaderPinDirections();
static inline uint8_t GetBootUart();
static inline void    InitBootUart();
static inline void    SuppressLineNoise();
static inline void    InitClock();

static uint8_t bootuart = 0;

/* main program starts here */
int main(void)
{
    InitClock();

    CheckWatchDogAtStartup();

    SetBootloaderPinDirections();
    bootuart = GetBootUart();
    InitBootUart();
    SuppressLineNoise();

    /* set LED pin as output */
#if defined __AVR_ATxmega128A1__
#define TOTEMPOLE      0x00  // Totempole
#define BUSKEEPER      0x08  // Buskeeper
#define WIRED_AND_PULL 0x38  // Wired-AND-PullUp
#define OUT_PULL_CONFIG TOTEMPOLE
//#define OUT_PULL_CONFIG BUSKEEPER
//#define OUT_PULL_CONFIG WIRED_AND_PULL

    PORTE.PIN0CTRL = OUT_PULL_CONFIG;

    // TODO: Need code
    PORTE.DIR = 0xFF;
    PORTE.OUT = 0xFF;
#else
    LED_DDR |= _BV(LED);
#endif

    /* flash onboard LED to signal entering of bootloader */
    flash_led(LED_FLASHES_AT_BOOT);

    /* forever loop */
    for (;;) {
        /* get character from UART */
        register uint8_t ch = getch();

        HandleChar(ch);
    } /* end of forever loop */

}

/* some variables */
union address_union {
    uint16_t word;
    uint8_t  byte[2];
} address;

union length_union {
    uint16_t word;
    uint8_t  byte[2];
} length;

struct flags_struct {
    unsigned eeprom : 1;
    unsigned rampz  : 1;
} flags;

uint8_t buff[LOADER_BUFF_SIZE];
 
uint8_t error_count = 0;

void (*app_start)(void) = 0x0000;

void InitClock() {
#if defined OSC_RC32MEN_bm
    // Enable 32M internal crystal
    OSC.CTRL |= OSC_RC32MEN_bm;
    // Wait for 32M internal crystal to stablize
    while ( !(OSC.STATUS & OSC_RC32MEN_bm) ) ;

#if 0
    // Does this go before or after internal 32M internal crystal in enabled and stable?
    // Dunno, try after first.

    // PLL source: internal 32M crystal multiplied by 4 - max PLL factor for 32M crystal.
    OSC.PLLCTRL = ((uint8_t)OSC_PLLSRC_RC2M_gc) | ( (4&OSC_PLLFAC_gm) << OSC_PLLFAC_gp);
    // Enable PLL
    OSC.CTRL |= OSC_PLLEN_bm;
    // Wait for PLL to stabilize
    while ( !(OSC.STATUS & OSC_PLLRDY_bm ) ) ;
#endif

#if 1
    // Set prescalars to 1, 1, 1
    RAMPZ = 0;
    asm volatile(
        "ldi  r30, lo8(%0)     \n\t"
        "ldi  r31, hi8(%0)     \n\t"
        "ldi  r16,  %2         \n\t"
        "out   %3, r16         \n\t"
        "st     Z,  %1         \n\t"
        :
        : "i" (&CLK.PSCTRL),
          "i" (CLK_PSADIV_1_gc | CLK_PSBCDIV_1_1_gc),
          "M" (CCP_IOREG_gc),
          "i" (&CCP)
        : "r16", "r30", "r31"
    );
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

#endif
}

#ifdef WATCHDOG_MODS
    static inline void CheckWatchDogAtStartup() {
        uint8_t ch = MCUSR;
        MCUSR = 0;

        WDTCSR |= _BV(WDCE) | _BV(WDE);
        WDTCSR = 0;

        // Check if the WDT was used to reset, in which case we dont bootload and skip straight
        // to the code. woot.
        if (! (ch &  _BV(EXTRF))) // if its not an external reset...
            app_start();  // skip bootloader
    }
#else
    static inline void CheckWatchDogAtStartup() {
        asm volatile("nop\n\t");
    }
#endif

// QUESTION: Do we even need SetBootloaderPinDirections
// QUESTION: or GetBootUart? I don't know of any arduinos
// QUESTION: that use BL0 or BL1.
static inline void SetBootloaderPinDirections() {
#if INIT_BL0_DIRECTION
    BL_DDR &= ~_BV(BL0);
    BL_PORT |= _BV(BL0);
#endif

#if INIT_BL1_DIRECTION
    BL_DDR &= ~_BV(BL1);
    BL_PORT |= _BV(BL1);
#endif
}

static inline uint8_t GetBootUart() {
#if INIT_BL0_DIRECTION
    /* check which UART should be used for booting */
    if(bit_is_clear(BL_PIN, BL0)) {
        return 1;
    }
#endif
#if INIT_BL1_DIRECTION
    else if(bit_is_clear(BL_PIN, BL1)) {
        return 2;
    }
#endif

    /* no UART was selected */
#if START_APP_IF_FLASH_PROGRAMED
    /* if flash is programmed already, start app, otherwise, start bootloader */
    if(pgm_read_byte_near(0x0000) == 0xFF) {
        app_start();
    }
#endif

    /* default to uart 0 */
    return 1;
}

static inline void InitBootUart() {
    if(bootuart == 1) {
        USART0_SET_DIR();
        USART0_SET_BAUD(BAUD_RATE);
// TODO: gc: Fix baud calc
//      USART0.BAUDCTRLA = 207; //  9600 baud with 32Mhz clock
        USART0.BAUDCTRLA = 52;  // 38400 baud with 32Mhz clock
        USART0.BAUDCTRLB = 0;
        USART0_RX_ENABLE();
        USART0_TX_ENABLE();

        USART0_SET_TO_8N1();
        return;
    }
#if defined BL1
    if(bootuart == 2) {
        UBRR1L = (uint8_t)(F_CPU/(BAUD_RATE*16L)-1);
        UBRR1H = (F_CPU/(BAUD_RATE*16L)-1) >> 8;
        UCSR1A = 0x00;
        UCSR1B = _BV(TXEN1)|_BV(RXEN1);
        UCSR1C = 0x06;
    }
    // bootuart should be set, if not, start app.
    app_start();
#endif
}

#if LINE_NOISE_PIN
    static inline void SuppressLineNoise() {
        /* Enable internal pull-up resistor on pin D0 (RX), in order
        to supress line noise that prevents the bootloader from
        timing out (DAM: 20070509) */
        DDR_LINE_NOISE  &= ~_BV(LINE_NOISE_PIN);
        PORT_LINE_NOISE |= _BV(LINE_NOISE_PIN);
    }
#else
    static inline void SuppressLineNoise() {
    }
#endif

void HandleChar(register int ch) {
    uint8_t  ch2;
    uint16_t w;

    /* A bunch of if...else if... gives smaller code than switch...case ! */

    /* Hello is anyone home ? */ 
    if(ch=='0') {
        nothing_response();
    }


// TODO: Kill this or restore it.
// We don't seem to need this, so save the bytes and kill it.
// If it turns out we do need it, then restore it.
// (gc 2010-01-21)
#if 0
    /* Request programmer ID */
    /* Not using PROGMEM string due to boot block in m128 being beyond 64kB boundry  */
    /* Would need to selectively manipulate RAMPZ, and it's only 9 characters anyway so who cares.  */
    else if(ch=='1') {
        if (getch() == ' ') {
            putch(0x14);
            putch('A');
            putch('V');
            putch('R');
            putch(' ');
            putch('I');
            putch('S');
            putch('P');
            putch(0x10);
        } else {
            if (++error_count == MAX_ERROR_COUNT)
                app_start();
        }
    }
#endif


    /* AVR ISP/STK500 board commands  DON'T CARE so default nothing_response */
    else if(ch=='@') {
        ch2 = getch();
        if (ch2>0x85) getch();
        nothing_response();
    }


    /* AVR ISP/STK500 board requests */
    else if(ch=='A') { // X
        ch2 = getch();
        if(ch2==0x80) byte_response(HW_VER);        // Hardware version
        else if(ch2==0x81) byte_response(SW_MAJOR); // Software major version
        else if(ch2==0x82) byte_response(SW_MINOR); // Software minor version
        else if(ch2==0x98) byte_response(0x03);     // Unknown but seems to be required by avr studio 3.56
        else byte_response(0x00);               // Covers various unnecessary responses we don't care about
    }


    /* Device Parameters  DON'T CARE, DEVICE IS FIXED  */
    else if(ch=='B') { // X
        getNch(20);
        nothing_response();
    }


    /* Parallel programming stuff  DON'T CARE  */
    else if(ch=='E') {
        getNch(5);
        nothing_response();
    }


    /* P: Enter programming mode  */
    /* R: Erase device, don't care as we will erase one page at a time anyway.  */
    else if(ch=='P' || ch=='R') {
        nothing_response();
    }


    /* Leave programming mode  */
    else if(ch=='Q') {
        nothing_response();
#ifdef WATCHDOG_MODS
        // autoreset via watchdog (sneaky!)
        WDTCSR = _BV(WDE);
        while (1); // 16 ms
#endif
    }


    /* Set address, little endian. EEPROM in bytes, FLASH in words  */
    /* Perhaps extra address bytes may be added in future to support > 128kB FLASH.  */
    /* This might explain why little endian was used here, big endian used everywhere else.  */
    else if(ch=='U') { // X
        address.byte[0] = getch();
        address.byte[1] = getch();
        nothing_response();
    }


    /* Universal SPI programming command, disabled.  Would be used for fuses and lock bits.  */
    else if(ch=='V') {
        getNch(4);
        byte_response(0x00);
    }

    /* Write memory, length is big endian and is in bytes  */
    else if(ch=='d') { // X
        length.byte[1] = getch();
        length.byte[0] = getch();
        flags.eeprom = 0;
        if (getch() == 'E') flags.eeprom = 1;
        for (w=0;w<length.word;w++) {
            buff[w] = getch();                          // Store data in buffer, can't keep up with serial data stream whilst programming pages
        }
        if (getch() == ' ') {
            if (flags.eeprom) {                     //Write to EEPROM one byte at a time
                address.word <<= 1;
                for(w=0;w<length.word;w++) {
#if USE_BUILT_IN_AVR_EEPROM_H
                    eeprom_write_byte((void *)address.word,buff[w]);
#elif defined EEPE && defined EEMPE && defined EEAR && defined EECR && defined EEDR
                    while(EECR & (1<<EEPE));
                    EEAR = (uint16_t)(void *)address.word;
                    EEDR = buff[w];
                    EECR |= (1<<EEMPE);
                    EECR |= (1<<EEPE);
#else
                    #error Do not know how to write to EEPROM
#endif
                    address.word++;
                }           
            }
            else {
                LoadProgram();
            }
            putch(0x14);
            putch(0x10);
        } else {
            if (++error_count == MAX_ERROR_COUNT)
                app_start();
        }       
    }


    /* Read memory block mode, length is big endian.  */
    else if(ch=='t') { // X
        length.byte[1] = getch();
        length.byte[0] = getch();
#if 16 < ADDR_BITS
        if (address.word>0x7FFF) flags.rampz = 1;       // No go with m256, FIXME
        else flags.rampz = 0;
#endif
        address.word = address.word << 1;           // address * 2 -> byte location
        if (getch() == 'E') flags.eeprom = 1;
        else flags.eeprom = 0;
        if (getch() == ' ') {                       // Command terminator
            putch(0x14);
            for (w=0;w < length.word;w++) {             // Can handle odd and even lengths okay
                if (flags.eeprom) {                         // Byte access EEPROM read
#if USE_BUILT_IN_AVR_EEPROM_H
                    putch(eeprom_read_byte((void *)address.word));
#else
                    while(EECR & (1<<EEPE));
                    EEAR = (uint16_t)(void *)address.word;
                    EECR |= (1<<EERE);
                    putch(EEDR);
#endif
                    address.word++;
                }
                else {

                    if (!flags.rampz) putch(pgm_read_byte_near(address.word));
#if 16 < ADDR_BITS
                    else putch(pgm_read_byte_far(address.word + 0x10000));
                    // Hmmmm, yuck  FIXME when m256 arrvies
#endif
                    address.word++;
                }
            }
            putch(0x10);
        }
    }


    /* Get device signature bytes  */
    else if(ch=='u') {
        if (getch() == ' ') { // X
            putch(0x14);
            putch(SIG1);
            putch(SIG2);
            putch(SIG3);
            putch(0x10);
        } else {
            if (++error_count == MAX_ERROR_COUNT)
                app_start();
        }
    }


    /* Read oscillator calibration byte */
    else if(ch=='v') {
        byte_response(0x00);
    }


#if defined MONITOR 

    /* here come the extended monitor commands by Erik Lins */

    /* check for three times exclamation mark pressed */
    else if(ch=='!') {
        ch = getch();
        if(ch=='!') {
        ch = getch();
        if(ch=='!') {
#if 16 < ADDR_BITS
            uint16_t extaddr;
#endif
            uint8_t addrl, addrh;

            const char* welcome = MONITOR_WELCOME;

            /* turn on LED */
            LED_DDR |= _BV(LED);
            LED_PORT &= ~_BV(LED);

            /* print a welcome message and command overview */
            int i;
            for(i=0; welcome[i] != '\0'; ++i) {
                putch(welcome[i]);
            }

            /* test for valid commands */
            for(;;) {
                putch('\n');
                putch('\r');
                putch(':');
                putch(' ');

                ch = getch();
                putch(ch);

                /* toggle LED */
                if(ch == 't') {
                    if(bit_is_set(LED_PIN,LED)) {
                        LED_PORT &= ~_BV(LED);
                        putch('1');
                    } else {
                        LED_PORT |= _BV(LED);
                        putch('0');
                    }
                } 

                /* read byte from address */
                else if(ch == 'r') {
                    ch = getch(); putch(ch);
                    addrh = gethex();
                    addrl = gethex();
                    putch('=');
                    ch = *(uint8_t *)((addrh << 8) + addrl);
                    puthex(ch);
                }

                /* write a byte to address  */
                else if(ch == 'w') {
                    ch = getch(); putch(ch);
                    addrh = gethex();
                    addrl = gethex();
                    ch = getch(); putch(ch);
                    ch = gethex();
                    *(uint8_t *)((addrh << 8) + addrl) = ch;
                }

                /* read from uart and echo back */
                else if(ch == 'u') {
                    for(;;) {
                        putch(getch());
                    }
                }
#if 16 < ADDR_BITS
                /* external bus loop  */
                else if(ch == 'b') {
                    putch('b');
                    putch('u');
                    putch('s');
                    MCUCR = 0x80;
                    XMCRA = 0;
                    XMCRB = 0;
                    extaddr = 0x1100;
                    for(;;) {
                        ch = *(volatile uint8_t *)extaddr;
                        if(++extaddr == 0) {
                            extaddr = 0x1100;
                        }
                    }
                }
#endif

                else if(ch == 'j') {
                    app_start();
                }

            } /* end of monitor functions */

        }
        }
    }
    /* end of monitor */
#endif
// TODO: Restore this branch to app on too many errors
// It hangs upload to sanguino for some reason.
// (gc 2010-01-21)
#if 0
    else if (++error_count == MAX_ERROR_COUNT) {
        app_start();
    }
#endif
}

extern void Spm(uint8_t code, uint16_t addr, uint16_t value);

void LoadProgram() {
    uint8_t address_high = 0;

    //Write to FLASH one page at a time
     address_high = address.byte[1]>127; //Only possible with m128, m256 will need 3rd address byte. FIXME
#if 16 < ADDR_BITS
     RAMPZ = address_high;
#endif
     address.word = address.word << 1;          //address * 2 -> byte location
     cli();                           //Disable interrupts, just to be sure
#if defined EEWE
     while(bit_is_set(EECR,EEWE));    //Wait for previous EEPROM writes to complete
#elif defined EEPE
     while(bit_is_set(EECR,EEPE));    //Wait for previous EEPROM writes to complete
//#elif
    // TODO: Do we need anything for xmegas?
#endif

    //Even up an odd number of bytes
    if (length.word & 0x01) {
        length.word++;  
    }
    int bytes = length.word;

    uint16_t* bufNext = (uint16_t*)buff;
    uint16_t  addr    = address.word;
    while (0 < bytes) {
        uint16_t page = addr;
        // Erase page pointed to by Z
        Spm( SPM_ERASE_PG, page, 0 ); // Erase page
#if defined SPM_RWW_EN
        Spm( SPM_RWW_EN, 0,    0 ); // Re-enable RWW section
#endif

        // Load words into FLASH page buffer
        int index;
        for ( index = PAGE_SIZE; 0 != index; --index ) {
            Spm( SPM_LOAD_WORD, addr, *bufNext ); // Load bufNext to address
            ++bufNext;
            addr  += 2;
            bytes -= 2;
        }

        Spm( SPM_WRITE_PG, page, 0 ); // Write page
#if defined SPM_RWW_EN
        Spm( SPM_RWW_EN, 0,    0 ); // Re-enable RWW section
#endif

// TODO: Get rid of this stupid return, or figure out why it is needed.
// It seems like we are writing only the first page. Yet all pages
// are uploaded?!?!? Is caller calling us once per page? I thought I
// tested and saw caller passing 128 bytes, two pages, on the 328p.
return;
    }
}

char gethexnib(void) {
    char a;
    a = getch(); putch(a);
    if(a >= 'a') {
        return (a - 'a' + 0x0a);
    } else if(a >= '0') {
        return(a - '0');
    }
    return a;
}


char gethex(void) {
    return (gethexnib() << 4) + gethexnib();
}


void puthex(char ch) {
    char ah;

    ah = ch >> 4;
    if(ah >= 0x0a) {
        ah = ah - 0x0a + 'a';
    } else {
        ah += '0';
    }
    
    ch &= 0x0f;
    if(ch >= 0x0a) {
        ch = ch - 0x0a + 'a';
    } else {
        ch += '0';
    }
    
    putch(ah);
    putch(ch);
}


void putch(char ch)
{
    if(bootuart == 1) {
        while ( !USART0_IS_TX_READY() );
        USART0_PUT_CHAR(ch);
    }
#if defined BL1
    else if (bootuart == 2) {
        while (!(UCSR1A & _BV(UDRE1)));
        UDR1 = ch;
    }
#endif
}


char getch(void)
{
    uint32_t count = 0;
    if(bootuart == 1) {
        while( !USART0_IS_RX_READY() ) {
            count++;
#if 0
            if (count > MAX_TIME_COUNT) {
                app_start();
            }
#endif
        }
        return USART0_GET_CHAR();
    }

#if defined BL1
    else if(bootuart == 2) {
        while(!(UCSR1A & _BV(RXC1))) {
            /* 20060803 DojoCorp:: Addon coming from the previous Bootloader*/               
            /* HACKME:: here is a good place to count times*/
            count++;
            if (count > MAX_TIME_COUNT) {
                app_start();
            }
        }
        return UDR1;
    }
#endif
    return 0;
}


void getNch(uint8_t count)
{
    while(count--) {
        getch();
    }

#if 0
// QUESTION: Should this code be removed???
/* Prior code was twisty. Why didn't mega128 and 1280 just call getch()
   But 1281 did? For now, just call getch, but keep this visible for a
   little while until we're sure the above works (gc 2010-01-21).
*/

    while(count--) {
#if defined(__AVR_ATmega128__) || defined(__AVR_ATmega1280__)
        if(bootuart == 1) {
            while(!(UCSR0A & _BV(RXC0)));
            UDR0;
        } 
        else if(bootuart == 2) {
            while(!(UCSR1A & _BV(RXC1)));
            UDR1;
        }
#elif defined UCSR0A
        getch();
#else
        /* m8,16,32,169,8515,8535,163 */
        /* 20060803 DojoCorp:: Addon coming from the previous Bootloader*/               
        //while(!(UCSRA & _BV(RXC)));
        //UDR;
        getch(); // need to handle time out
#endif      
    }
#endif // 0
}


void byte_response(uint8_t val)
{
    if (getch() == ' ') {
        putch(0x14);
        putch(val);
        putch(0x10);
    } else {
        if (++error_count == MAX_ERROR_COUNT)
            app_start();
    }
}


void nothing_response(void)
{
    if (getch() == ' ') {
        putch(0x14);
        putch(0x10);
    } else {
        if (++error_count == MAX_ERROR_COUNT)
            app_start();
    }
}

void delay_ms(uint32_t count)
{
    count *= 75;
    while (count--) {
        _delay_ms(1);
    }
}

void flash_led(uint8_t count)
{
#if defined __AVR_ATxmega128A1__
    // TODO: Need to abstract and not use cpu macro.
    // TODO: Need code
    while (count--) {
        PORTE.OUTTGL = 0xFF;
        delay_ms(200);
        PORTE.OUTTGL = 0xFF;
        delay_ms(200);
    }
#else
    while (count--) {
        LED_PORT |= _BV(LED);
        _delay_ms(200);
        LED_PORT &= ~_BV(LED);
        _delay_ms(200);
    }
#endif
}

/* end of file ATmegaBOOT.c */
