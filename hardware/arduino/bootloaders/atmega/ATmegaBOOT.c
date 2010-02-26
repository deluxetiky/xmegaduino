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
#define PAGE_BYTES (PAGE_SIZE*2)
#define DIAG_ENABLE 1
// Addresses seem to be byte oriented?!?!?!?!
#define ADDRESS_IN_WORDS 0

/* function prototypes */
void putch(char);
char getch(void);
void getNch(uint8_t count);
void byte_response(uint8_t value);
void nothing_response(void);
char gethex(void);
void puthex(char value);
void flash_led(uint8_t count);
void InitLed(void);
void HandleChar(int c);
void LoadProgram(void);
void delay_ms(uint32_t count);

extern void Spm(uint8_t code, uint16_t addr, uint16_t value);

static inline void    CheckWatchDogAtStartup(void);
static inline void    SetBootloaderPinDirections(void);
static inline uint8_t GetBootUart(void);
static inline void    InitBootUart(void);
static inline void    SuppressLineNoise(void);
static inline void    InitClock(void);

#if 1 <= DIAG_ENABLE
    static void DiagEnable(void);
    static void DiagChar(char value);
    static void Diag(const char *value);
    static void DiagNumber(unsigned long value, uint8_t base);
#else
    #define DiagEnable()
#endif

// TODO: kill bootuart. Keep pointer to boot usart.
static uint8_t bootuart = -1;

/* main program starts here */
int main(void)
{
    CheckWatchDogAtStartup();
    InitClock();
    DiagEnable();
    InitLed();
    /* flash onboard LED to signal entering of bootloader */
    flash_led(LED_FLASHES_AT_BOOT);
    SetBootloaderPinDirections();
    bootuart = GetBootUart();
    InitBootUart();
    SuppressLineNoise();
Diag("Boot UART: "); DiagNumber(bootuart,10); Diag("\n"); delay_ms(1);

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

#define SET_PRESCALARS_TO_1_1_1 0 // Should already be 1,1,1. Default at start.
#if SET_PRESCALARS_TO_1_1_1
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

    // Set main system clock to 32Mhz clock
    register uint8_t value = (CLK.CTRL & ~CLK_SCLKSEL_gm ) | CLK_SCLKSEL_RC32M_gc;
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

static inline void SetBootloaderPinDirections() {
    #if defined BL_PORT
        PORTCFG.MPCMASK = 0xFF; //do this for all pins of the following command
        BL_PORT.PIN0CTRL = WIRED_AND_PULL;
    #endif
    
    #if defined BL_0_PIN 
        BL_DIR &= ~_BV(BL_0_PIN);
        BL_OUT |=  _BV(BL_0_PIN);
    #endif

    #if defined BL_1_PIN
        BL_DIR &= ~_BV(BL_1_PIN);
        BL_OUT |=  _BV(BL_1_PIN);
    #endif

    #if defined BL_2_PIN
        BL_DIR &= ~_BV(BL_2_PIN);
        BL_OUT |=  _BV(BL_2_PIN);
    #endif

    #if defined APP_PIN
        BL_DIR &= ~_BV(APP_PIN);
        BL_OUT |=  _BV(APP_PIN);
    #endif
}

// TODO: kill bootuart. Keep pointer to boot usart.
static inline uint8_t GetBootUart() {

    #if defined APP_PIN
        while (1) {
    #endif

PORTE.OUT = ~(uint8_t)PORTF.IN;
    /* check which UART should be used for booting */
    #if defined BL_0_PIN
        if(bit_is_clear(BL_IN, BL_0_PIN)) {
            return 0;
        }
    #endif
    #if defined BL_1_PIN
        if(bit_is_clear(BL_IN, BL_1_PIN)) {
            return 1;
        }
    #endif
    #if defined BL_2_PIN
        if(bit_is_clear(BL_IN, BL_2_PIN)) {
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
    #if defined APP_PIN
        if(bit_is_clear(BL_IN, APP_PIN)) {
            app_start();
        }
    #endif

    #if defined APP_PIN
        }
    #endif

    /* default to uart 0 */
    return 0;
}

static inline void InitBootUart() {
    if(bootuart == 0) {
        USART0_SET_DIR();
        USART0_SET_BAUD(BAUD_RATE_0);
        USART0_RX_ENABLE();
        USART0_TX_ENABLE();
        USART0_SET_TO_8N1();
// TODO: gc: Fix baud calc
        USART_0.BAUDCTRLA = 207; //   9600 baud with 32Mhz clock
//      USART_0.BAUDCTRLA =  52; //  38400 baud with 32Mhz clock
//      USART_0.BAUDCTRLA =  33; //  57600 baud with 32Mhz clock
//      USART_0.BAUDCTRLA =  17; // 115200 baud with 32Mhz clock
        USART_0.BAUDCTRLB =   0;
        return;
    }
#if defined BL_1_PIN
    if(bootuart == 1) {
        USART1_SET_DIR();
        USART1_SET_BAUD(BAUD_RATE_1);
        USART1_RX_ENABLE();
        USART1_TX_ENABLE();
        USART1_SET_TO_8N1();
//      USART_1.BAUDCTRLA =  52; //  38400 baud with 32Mhz clock
        USART_1.BAUDCTRLA =  33; //  57600 baud with 32Mhz clock
        USART_1.BAUDCTRLB =   0;
        return;
    }
#endif
#if defined BL_2_PIN
    if(bootuart == 2) {
        USART2_SET_DIR();
        USART2_SET_BAUD(BAUD_RATE_2);
        USART2_RX_ENABLE();
        USART2_TX_ENABLE();
        USART2_SET_TO_8N1();
//      USART_2.BAUDCTRLA =  52; //  38400 baud with 32Mhz clock
        USART_2.BAUDCTRLA =  33; //  57600 baud with 32Mhz clock
        USART_2.BAUDCTRLB =   0;
        return;
    }
#endif
    // bootuart should be set, if not, start app.
    app_start();
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

int program_load_in_progress = 0;

void HandleChar(register int ch) {
    uint8_t  ch2;
    uint16_t w;

    uint16_t addr = address.word;
    uint16_t page = addr & ~(PAGE_BYTES-1);
    if ( program_load_in_progress && addr != page ) {
        // Write partial page at end of program
        Spm( SPM_WRITE_PG, page, 0 ); // Write page
        program_load_in_progress = 0;
    }

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

// TODO: Kill this or restore it.
// We don't seem to need this, so save the bytes and kill it.
// If it turns out we do need it, then restore it.
// (gc 2010-02-22)
#if 0
    /* AVR ISP/STK500 board commands  DON'T CARE so default nothing_response */
    else if(ch=='@') {
        ch2 = getch();
        if (ch2>0x85) getch();
        nothing_response();
    }
#endif

    /* AVR ISP/STK500 board requests */
    else if(ch=='A') {
        ch2 = getch();
        if(ch2==0x80) byte_response(HW_VER);        // Hardware version
        else if(ch2==0x81) byte_response(SW_MAJOR); // Software major version
        else if(ch2==0x82) byte_response(SW_MINOR); // Software minor version
        else if(ch2==0x98) byte_response(0x03);     // Unknown but seems to be required by avr studio 3.56
        else byte_response(0x00);               // Covers various unnecessary responses we don't care about
    }


    /* Device Parameters  DON'T CARE, DEVICE IS FIXED  */
    else if(ch=='B') {
        getNch(20);
        nothing_response();
    }


// TODO: Kill this or restore it.
// We don't seem to need this, so save the bytes and kill it.
// If it turns out we do need it, then restore it.
// (gc 2010-02-22)
#if 0
    /* Parallel programming stuff  DON'T CARE  */
    else if(ch=='E') {
        getNch(5);
        nothing_response();
    }
#endif

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
#else
		delay_ms(3);
        app_start();
#endif
    }

// TODO: Flash seems to be in bytes!!!!
    /* Set address, little endian. EEPROM in bytes, FLASH in words  */
    /* Perhaps extra address bytes may be added in future to support > 128kB FLASH.  */
    /* This might explain why little endian was used here, big endian used everywhere else.  */
    else if(ch=='U') {
        address.byte[0] = getch();
        address.byte[1] = getch();
        nothing_response();
    }


// TODO: Kill this or restore it.
// We don't seem to need this, so save the bytes and kill it.
// If it turns out we do need it, then restore it.
// (gc 2010-02-22)
#if 0
    /* Universal SPI programming command, disabled.  Would be used for fuses and lock bits.  */
    else if(ch=='V') {
        getNch(4);
        byte_response(0x00);
    }
#endif

    /* Write memory, length is big endian and is in bytes  */
    else if(ch=='d') {
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
    else if(ch=='t') {
        length.byte[1] = getch();
        length.byte[0] = getch();
#if ADDRESS_IN_WORDS
#if 16 < ADDR_BITS
        if (address.word>0x7FFF) flags.rampz = 1;       // No go with m256, FIXME
        else flags.rampz = 0;
#endif
        address.word = address.word << 1;           // address * 2 -> byte location
#endif
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
        if (getch() == ' ') {
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

// TODO: Kill this or restore it.
// We don't seem to need this, so save the bytes and kill it.
// If it turns out we do need it, then restore it.
// (gc 2010-02-22)
#if 0
    /* Read oscillator calibration byte */
    else if(ch=='v') {
        byte_response(0x00);
    }
#endif

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

void LoadProgram() {

    program_load_in_progress = 1;

#if ADDRESS_IN_WORDS
    uint8_t address_high = 0;

    //Write to FLASH one page at a time
    address_high = address.byte[1]>127; //Only possible with m128, m256 will need 3rd address byte. FIXME
#if 16 < ADDR_BITS
    RAMPZ = address_high;
#endif
    address.word = address.word << 1; // word -> byte
#endif
    cli();                            // Disable interrupts, just to be sure
    WAIT_FOR_EPROM_WRITE;             // Wait for previous EEPROM writes to complete

#if ADDRESS_IN_WORDS
    int bytes = length.word << 1;
#else
    int bytes = length.word;
#endif

    uint16_t* bufNext = (uint16_t*)buff;
    uint16_t  addr    = address.word;
    while (0 < bytes) {
        uint16_t page = addr & ~(PAGE_BYTES-1);
        // Erase page pointed to by Z
        if ( addr == page ) {
            Spm( SPM_ERASE_PG, page, 0 ); // Erase page
            ENABLE_RWW; // Re-enable RWW section
        }


        // Load words into FLASH page buffer
        int index;
        int count = PAGE_BYTES;
        if ( bytes < PAGE_BYTES ) {
            count = bytes;
        }
        for ( index = count; 0 < index; index -= 2 ) {
            Spm( SPM_LOAD_WORD, addr, *bufNext ); // Load bufNext to address
            ++bufNext;
            addr  += 2;
            bytes -= 2;
        }

        if ( page + PAGE_BYTES <= addr ) {
            Spm( SPM_WRITE_PG, page, 0 ); // Write page
            ENABLE_RWW;                   // Re-enable RWW section
        }

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

int is_boot_uart_tx_ready()
{
    if (bootuart == 0) {
        return USART0_IS_TX_READY();
    }
    #if defined BL_1_PIN
        if (bootuart == 1) {
            return USART1_IS_TX_READY();
        }
    #endif
    #if defined BL_2_PIN
        if (bootuart == 2) {
            return USART2_IS_TX_READY();
        }
    #endif
    return 0;
}

void boot_uart_put_char(char value)
{
    if (bootuart == 0) {
        USART0_PUT_CHAR(value);
    }
    #if defined BL_1_PIN
        if (bootuart == 1) {
            USART1_PUT_CHAR(value);
        }
    #endif
    #if defined BL_2_PIN
        if (bootuart == 2) {
            USART2_PUT_CHAR(value);
        }
    #endif
}


void putch(char ch)
{
    while ( !is_boot_uart_tx_ready() );
    boot_uart_put_char(ch);
}

int is_boot_uart_rx_ready()
{
    if (bootuart == 0) {
        return USART0_IS_RX_READY();
    }
    #if defined BL_1_PIN
        if (bootuart == 1) {
            return USART1_IS_RX_READY();
        }
    #endif
    #if defined BL_2_PIN
        if (bootuart == 2) {
            return USART2_IS_RX_READY();
        }
    #endif
    return 0;
}

char boot_uart_get_char()
{
    if (bootuart == 0) {
        return USART0_GET_CHAR();
    }
    #if defined BL_1_PIN
        if (bootuart == 1) {
            return USART1_GET_CHAR();
        }
    #endif
    #if defined BL_2_PIN
        if (bootuart == 2) {
            return USART2_GET_CHAR();
        }
    #endif
    return 0;
}

char getch(void)
{
    uint32_t count = 0;
    while ( !is_boot_uart_rx_ready() ) {
        #if 1 <= DIAG_ENABLE
            PORTE.OUT = ~(uint8_t)(count >> 17);
        #endif
        count++;
        #if !defined APP_PIN
            if (count > MAX_TIME_COUNT) {
                app_start();
            }
        #endif
    }
    return boot_uart_get_char();

    return 0;
}


void getNch(uint8_t count)
{
    while(count--) {
        getch();
    }
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
    count *= 1; // 75;
    while (count--) {
        _delay_ms(1);
    }
}

void InitLed(void)
{
    /* set LED pin as output */
    #if xplain == TARGET
        PORTCFG.MPCMASK = 0xFF; //do this for all pins of the following command
        PORTE.PIN0CTRL = WIRED_AND_PULL;

        PORTE.DIR      = 0xFF;
        PORTE.OUT      = 0xFF;
    #else
        LED_DDR |= _BV(LED);
    #endif
}

void flash_led(uint8_t count)
{
#if xplain == TARGET
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

#if 1 <= DIAG_ENABLE

    // TODO: Move to someplace else
    #define USART_PORT_DIAG PORTD
    #define USART_DIAG USARTD1
    #define USART_DIAG_SET_DIR() \
            USART_PORT_DIAG.DIRSET = PIN7_bm; \
            USART_PORT_DIAG.DIRCLR = PIN6_bm;
    #define USART_DIAG_SET_TO_8N1()   \
            USART_DIAG.CTRLC = USART_CHSIZE_8BIT_gc | USART_PMODE_DISABLED_gc
    #define USART_DIAG_TX_ENABLE() USART_DIAG.CTRLB |= USART_TXEN_bm;
    #define USART_DIAG_IS_TX_READY() ( (USART_DIAG.STATUS & USART_DREIF_bm) != 0)
    #define USART_DIAG_PUT_CHAR(c) (USART_DIAG.DATA = c)

    void DiagEnable()
    {
      USART_DIAG_SET_DIR();
      USART_DIAG_SET_TO_8N1();
    //USART_DIAG.BAUDCTRLA = 207; // 9600
    //USART_DIAG.BAUDCTRLA =  52; // 38400
      USART_DIAG.BAUDCTRLA =  33; // 57600
      USART_DIAG_TX_ENABLE();
    }

    void DiagChar(char c)
    {
      while ( !USART_DIAG_IS_TX_READY() );
      USART_DIAG_PUT_CHAR(c);
    }

    void Diag(const char *str)
    {
      while (*str)
        DiagChar(*str++);
    }

    void DiagNumber(unsigned long n, uint8_t base)
    {
      unsigned char buf[8 * sizeof(long)]; // Assumes 8-bit chars. 
      unsigned long i = 0;

      if (n == 0) {
        Diag("0");
        return;
      } 

      while (n > 0) {
        buf[i++] = n % base;
        n /= base;
      }

      for (; i > 0; i--)
        DiagChar((char) (buf[i - 1] < 10 ?
          '0' + buf[i - 1] :
          'A' + buf[i - 1] - 10));
    }

#endif

/* end of file ATmegaBOOT.c */
