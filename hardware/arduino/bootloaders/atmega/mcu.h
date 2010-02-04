#ifndef MCU_H
#define MCU_H

#if defined __AVR_ATmega168__ || defined __AVR_ATmega328P__
    #define USE_BUILT_IN_AVR_EEPROM_H 1
#endif

#ifdef __AVR_ATmega128__
    #define START_APP_IF_FLASH_PROGRAMED 1
#endif

#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__)
    #define LINE_NOISE_PIN  PIND0
    #define DDR_LINE_NOISE  DDRD
    #define PORT_LINE_NOISE PORTD
#elif defined __AVR_ATmega1280__
    #define LINE_NOISE_PIN  PINE0
    #define DDR_LINE_NOISE  DDRE
    #define PORT_LINE_NOISE PORTE
#endif

#if defined __AVR_ATxmega128A1__
    #define USART0       USARTC0
    #define USART_PORT_0 PORTC
#endif

// QUESTION: Do we even need BL0, BL1, etc? I don't know of any arduinos
// QUESTION: that use them.

/* Adjust to suit whatever pin your hardware uses to enter the bootloader */
/* ATmega128 has two UARTS so two pins are used to enter bootloader and select UART */
/* ATmega1280 has four UARTS, but for Arduino Mega, we will only use RXD0 to get code */
/* BL0... means UART0, BL1... means UART1 */
#ifdef __AVR_ATmega128__
    #define BL_DDR  DDRF
    #define BL_PORT PORTF
    #define BL_PIN  PINF
    #define BL0     PINF7
    #define BL1     PINF6
    #define INIT_BL0_DIRECTION 1
    #define INIT_BL1_DIRECTION 1
#elif defined __AVR_ATmega1280__ 
    /* we just don't do anything for the MEGA and enter bootloader on reset anyway*/
#endif

#if defined __AVR_ATmega128__ || defined __AVR_ATmega1280__
    #define LED_FLASHES_AT_BOOT NUM_LED_FLASHES + bootuart
#endif

#if defined __AVR_ATmega163__
    #define SPM_POST ".word 0xFFFF \n\t" \
                     "nop          \n\t"
#endif

#endif // MCU_H
