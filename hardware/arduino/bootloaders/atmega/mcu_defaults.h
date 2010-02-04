#ifndef MCU_DEFAULTS_H
#define MCU_DEFAULTS_H

#include <avr/io.h>

#include "mcu.h"

/* UART names. Settle on UBRRnL convention, remapping those that use UBRRL. */
#if !defined UBRR0L && defined UBRRL
    // UART speed low
    #define UBRR0L UBRRL
    // UART speed high
    #define UBRR0H UBRRH  
    // UCSR0A seems to be double speed mode
    // RXEN, TXEN
    #define UCSR0B UCSRB
    // 8N1
    #define UCSR0C UCSRC
    #define TXEN0  TXEN
    #define RXEN0  RXEN
#endif

#if !defined UCSR0A && defined UCSRA
    #define UCSR0A UCSRA
#endif

#define USART0_BSEL_NEG(n,scale) (F_CPU/(16L*n)*(2^scale)-1)
#define USART0_BSEL_POS(n,scale) (F_CPU/(16L*n)/(2^scale)-1)
#define USART0_BSEL(n,scale) \
        ( 0<=(scale) ? USART0_BSEL_POS(n,scale) : USART0_BSEL_NEG(n,-scale) )

#if defined UBRR0L
    #define USART0_SET_DIR()

    #if defined UCSRC
        #define USART0_SET_TO_8N1()   \
            UCSRC = (1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);
    #elif defined UCSR0C
        #define USART0_SET_TO_8N1()   \
            UCSR0C = (1<<UCSZ00) | (1<<UCSZ01);
    #else
        #error Do not know how to set up UART to 8N1
    #endif

    #if defined DOUBLE_SPEED
        #define USART0_SET_BAUD(n) \
            UBRR0L = (uint8_t)(F_CPU/(n*8L)-1); \
            UBRR0H = (F_CPU/(n*8L)-1) >> 8; \
            UCSR0A = (1<<U2X0); //Double speed mode USART0
    #else
        #define USART0_SET_BAUD(n) \
            UBRR0L = (uint8_t)(F_CPU/(n*16L)-1); \
            UBRR0H = (F_CPU/(n*16L)-1) >> 8; \
            UCSR0A = 0x00;
    #endif

    #define USART0_RX_ENABLE() UCSR0B |= _BV(RXEN0);
            
    #define USART0_TX_ENABLE() UCSR0B |= _BV(TXEN0);

    #define USART0_IS_TX_READY() (UCSR0A & _BV(UDRE0))

    #define USART0_PUT_CHAR(c) (UDR0 = c)

    #define USART0_IS_RX_READY() (UCSR0A & _BV(RXC0))

    #define USART0_GET_CHAR() (UDR0)
#elif defined USARTC0
    #define USART0_SET_DIR() \
            USART_PORT_0.DIRSET = PIN3_bm; \
            USART_PORT_0.DIRCLR = PIN2_bm;

    #define USART0_SET_TO_8N1()   \
            USART0.CTRLC = USART_CHSIZE_8BIT_gc | USART_PMODE_DISABLED_gc

    #define USART0_SET_BAUD_WITH_SCALE(n,scale) \
            USART0.BAUDCTRLA = (uint8_t)(USART0_BSEL(n,scale)); \
            USART0.BAUDCTRLB = (scale << USART_BSCALE0_bp)|(USART0_BSEL(n,scale)>>8);
    #define USART0_SET_BAUD(n) USART0_SET_BAUD_WITH_SCALE(n,0) \

    #define USART0_RX_ENABLE() USART0.CTRLB |= USART_RXEN_bm;
            
    #define USART0_TX_ENABLE() USART0.CTRLB |= USART_TXEN_bm;

    #define USART0_IS_TX_READY() ( (USART0.STATUS & USART_DREIF_bm) != 0)

    #define USART0_PUT_CHAR(c) (USART0.DATA = c)

    #define USART0_IS_RX_READY() ( (USART0.STATUS & USART_RXCIF_bm) != 0)

    #define USART0_GET_CHAR() (USART0.DATA)
#else
    #error Do not know how to set up UART
#endif

#if !defined LINE_NOISE_PIN
    #define LINE_NOISE_PIN 0
    #define DDR_LINE_NOISE 0
    #define PORT_LINE_NOISE 0
#endif

// QUESTION: Do we even need BL0, BL1, etc? I don't know of any arduinos
// QUESTION: that use them.

/* Bootloader pins. */
/* Adjust to suit whatever pin your hardware uses to enter the bootloader */
/* ATmega128 has two UARTS so two pins are used to enter bootloader and select UART */
/* ATmega1280 has four UARTS, but for Arduino Mega, we will only use RXD0 to get code */
/* BL0... means UART0, BL1... means UART1 */
#if !defined BL0
    #define BL_DDR  DDRD
    #define BL_PORT PORTD
    #define BL_PIN  PIND
    #define BL0     PIND6
#endif

#if !defined INIT_BL0_DIRECTION
    /* We run the bootloader regardless of the state of this pin.  Thus, don't
    put it in a different state than the other pins.  --DAM, 070709
    This also applies to Arduino Mega -- DC, 080930
    */
    #define INIT_BL0_DIRECTION 0
#endif
#if !defined INIT_BL1_DIRECTION
    #define INIT_BL1_DIRECTION 0
#endif

#if !defined USE_BUILT_IN_AVR_EEPROM_H 
    #define USE_BUILT_IN_AVR_EEPROM_H 1
#endif

/* the current avr-libc eeprom functions do not support the ATmega168 */
/* own eeprom write/read functions are used instead */
#if USE_BUILT_IN_AVR_EEPROM_H 
    #include <avr/eeprom.h>
#endif

#if !defined EEWE && defined EEPE
    #define EEWE EEPE
#endif

#if !defined SPM_POST
    #define SPM_POST
#endif

#if defined SPMCSR
    #define SPM_STATUS    SPMCSR
    #define SPM_CMD       SPMCSR
    #define SPM_BUSY      1
    #define SPM_LOAD_WORD 0x01
    #define SPM_ERASE_PG  0x03
    #define SPM_WRITE_PG  0x05
    #define SPM_RWW_EN    0x11
#elif defined SPMCR
    #define SPM_STATUS    SPMCR
    #define SPM_CMD       SPMCR
    #define SPM_BUSY      1
    #define SPM_LOAD_WORD 0x01
    #define SPM_ERASE_PG  0x03
    #define SPM_WRITE_PG  0x05
    #define SPM_RWW_EN    0x11
#elif defined NVM_STATUS
    #define SPM_STATUS    NVM_STATUS
    #define SPM_CMD       NVM_CMD
    #define SPM_BUSY      NVM_NVMBUSY_bm
    #define SPM_LOAD_WORD NVM_CMD_LOAD_FLASH_BUFFER_gc
    #define SPM_ERASE_PG  NVM_CMD_ERASE_APP_PAGE_gc
    #define SPM_WRITE_PG  NVM_CMD_WRITE_APP_PAGE_gc
#else
    #error MCU does not support serial bootloading
#endif

#endif // MCU_DEFAULTS_H
