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

#define USART_BSEL_NEG(n,scale) ( (uint16_t)(F_CPU/(16L*n)*(2^scale)-1) )
#define USART_BSEL_POS(n,scale) ( (uint16_t)(F_CPU/(16L*n)/(2^scale)-1) )
#define USART_BSEL(n,scale) \
        ( 0<=(scale) ? USART_BSEL_POS(n,scale) : USART_BSEL_NEG(n,-scale) )

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
    #define USART_SET_DIR(port, read_pin, write_pin) \
            port.DIRCLR = _BV(read_pin); \
            port.DIRSET = _BV(write_pin);

    #define USART_SET_TO_8N1(usart)   \
            usart.CTRLC = USART_CHSIZE_8BIT_gc | USART_PMODE_DISABLED_gc

    #define USART_SET_BAUD_WITH_SCALE(usart,baud,scale) \
            usart.BAUDCTRLA = (uint8_t)(USART_BSEL(baud,scale)); \
            usart.BAUDCTRLB = (scale << USART_BSCALE0_bp)|(USART_BSEL(baud,scale)>>8);
    #define USART_SET_BAUD(usart, baud) USART_SET_BAUD_WITH_SCALE(usart,baud,0)

    #define USART_RX_ENABLE(usart) usart.CTRLB |= USART_RXEN_bm;

    #define USART_TX_ENABLE(usart) usart.CTRLB |= USART_TXEN_bm;

    #define USART_IS_TX_READY(usart) ( (usart.STATUS & USART_DREIF_bm) != 0)

    #define USART_IS_RX_READY(usart) ( (usart.STATUS & USART_RXCIF_bm) != 0)

    #define USART_PUT_CHAR(usart, c) (usart.DATA = c)

    #define USART_GET_CHAR(usart) (usart.DATA)


    #define USART0_SET_DIR()      USART_SET_DIR(USART_0_PORT,USART_0_RD_PIN,USART_0_WR_PIN) 
    #define USART0_SET_TO_8N1()   USART_SET_TO_8N1(USART_0) 
    #define USART0_SET_BAUD(baud) USART_SET_BAUD(USART_0,baud) 
    #define USART0_RX_ENABLE()    USART_RX_ENABLE(USART_0) 
    #define USART0_TX_ENABLE()    USART_TX_ENABLE(USART_0) 
    #define USART0_IS_TX_READY()  USART_IS_TX_READY(USART_0) 
    #define USART0_IS_RX_READY()  USART_IS_RX_READY(USART_0) 
    #define USART0_PUT_CHAR(c)    USART_PUT_CHAR(USART_0,c) 
    #define USART0_GET_CHAR()     USART_GET_CHAR(USART_0) 

    #define USART1_SET_DIR()      USART_SET_DIR(USART_1_PORT,USART_1_RD_PIN,USART_1_WR_PIN) 
    #define USART1_SET_TO_8N1()   USART_SET_TO_8N1(USART_1) 
    #define USART1_SET_BAUD(baud) USART_SET_BAUD(USART_1,baud) 
    #define USART1_RX_ENABLE()    USART_RX_ENABLE(USART_1) 
    #define USART1_TX_ENABLE()    USART_TX_ENABLE(USART_1) 
    #define USART1_IS_TX_READY()  USART_IS_TX_READY(USART_1) 
    #define USART1_IS_RX_READY()  USART_IS_RX_READY(USART_1) 
    #define USART1_PUT_CHAR(c)    USART_PUT_CHAR(USART_1,c) 
    #define USART1_GET_CHAR()     USART_GET_CHAR(USART_1) 

    #define USART2_SET_DIR()      USART_SET_DIR(USART_2_PORT,USART_2_RD_PIN,USART_2_WR_PIN) 
    #define USART2_SET_TO_8N1()   USART_SET_TO_8N1(USART_2) 
    #define USART2_SET_BAUD(baud) USART_SET_BAUD(USART_2,baud) 
    #define USART2_RX_ENABLE()    USART_RX_ENABLE(USART_2) 
    #define USART2_TX_ENABLE()    USART_TX_ENABLE(USART_2) 
    #define USART2_IS_TX_READY()  USART_IS_TX_READY(USART_2) 
    #define USART2_IS_RX_READY()  USART_IS_RX_READY(USART_2) 
    #define USART2_PUT_CHAR(c)    USART_PUT_CHAR(USART_2,c) 
    #define USART2_GET_CHAR()     USART_GET_CHAR(USART_2) 
#else
    #error Do not know how to set up UART
#endif

#if !defined LINE_NOISE_PIN
    #define LINE_NOISE_PIN 0
    #define DDR_LINE_NOISE 0
    #define PORT_LINE_NOISE 0
#endif

#if !defined USE_BUILT_IN_AVR_EEPROM_H 
    #define USE_BUILT_IN_AVR_EEPROM_H 1
#endif

/* the current avr-libc eeprom functions do not support the ATmega168 */
/* own eeprom write/read functions are used instead */
#if USE_BUILT_IN_AVR_EEPROM_H 
    #include <avr/eeprom.h>
#endif

#if defined EEWE
 	#define WAIT_FOR_EPROM_WRITE while(bit_is_set(EECR,EEWE))
#elif defined EEPE
 	#define WAIT_FOR_EPROM_WRITE while(bit_is_set(EECR,EEPE))
#elif defined __AVR_ATxmega128A1__
	// TODO: Figure out what to do with XMEGA
	#define WAIT_FOR_EPROM_WRITE
#else
    #error Do not know how to define WAIT_FOR_EPROM_WRITE
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

#if defined SPM_RWW_EN
    #define ENABLE_RWW Spm( SPM_RWW_EN, 0, 0 ); // Re-enable RWW section
#else
	#define ENABLE_RWW
#endif

#endif // MCU_DEFAULTS_H
