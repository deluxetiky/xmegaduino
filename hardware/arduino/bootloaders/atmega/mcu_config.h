#ifndef MCU_CONFIG_H
#define MCU_CONFIG_H

#if defined __AVR_ATmega168__ || defined __AVR_ATmega328P__
    #define USE_BUILT_IN_AVR_EEPROM_H 1
#endif

#if !defined(USE_BUILT_IN_AVR_EEPROM_H)
    #define USE_BUILT_IN_AVR_EEPROM_H 1
#endif

/* the current avr-libc eeprom functions do not support the ATmega168 */
/* own eeprom write/read functions are used instead */
#if USE_BUILT_IN_AVR_EEPROM_H 
#include <avr/eeprom.h>
#endif

/* Use the F_CPU defined in Makefile */

/* 20060803: hacked by DojoCorp */
/* 20070626: hacked by David A. Mellis to decrease waiting time for auto-reset */
/* set the waiting time for the bootloader */
/* Define if not defined by the Makefile */
#if ! defined MAX_TIME_COUNT 
#define MAX_TIME_COUNT (F_CPU>>4)
#endif

/* 20070707: hacked by David A. Mellis - after this many errors give up and launch application */
#define MAX_ERROR_COUNT 5

/* set the UART baud rate */
/* 20060803: hacked by DojoCorp */
//#define BAUD_RATE   115200
#ifndef BAUD_RATE
#define BAUD_RATE   19200
#endif


/* SW_MAJOR and MINOR needs to be updated from time to time to avoid warning message from AVR Studio */
/* never allow AVR Studio to do an update !!!! */
#define HW_VER   0x02
#define SW_MAJOR 0x01
#define SW_MINOR 0x10


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
#elif defined __AVR_ATmega1280__ 
/* we just don't do anything for the MEGA and enter bootloader on reset anyway*/
#endif

#if !defined(BL0)
/* other ATmegas have only one UART, so only one pin is defined to enter bootloader */
#define BL_DDR  DDRD
#define BL_PORT PORTD
#define BL_PIN  PIND
#define BL0     PIND6
#endif

/* onboard LED is used to indicate, that the bootloader was entered (3x flashing) */
/* if monitor functions are included, LED goes on after monitor was entered */

#if !defined LED_PORT
#define LED_DDR  DDRB
#define LED_PORT PORTB
#define LED_PIN  PINB
#endif

#if defined __AVR_ATmega128__ || defined __AVR_ATmega1280__
/* Onboard LED is connected to pin PB7 (e.g. Crumb128, PROBOmega128, Savvy128, Arduino Mega) */
#define LED      PINB7
#endif

#if defined __AVR_ATmega644__
#define LED      PINB0
#endif

#if !defined(LED)
/* Onboard LED is connected to pin PB5 in Arduino NG, Diecimila, and Duomilanuove */ 
/* other boards like e.g. Crumb8, Crumb168 are using PB2 */
#define LED      PINB5
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
    #define SPM_LOAD_WORD NVM_CMD_LOAD_FLASH_BUFFER_gc,
    #define SPM_ERASE_PG  NVM_CMD_ERASE_APP_PAGE_gc,
    #define SPM_WRITE_PG  NVM_CMD_WRITE_APP_PAGE_gc,
#else
    #error MCU does not support serial bootloading
#endif

#if defined __AVR_ATmega163__
    #define SPM_POST ".word 0xFFFF \n\t" \
                     "nop          \n\t"
#endif


/* monitor functions will only be compiled when using ATmega128, due to bootblock size constraints */
#if defined(__AVR_ATmega128__) || defined(__AVR_ATmega1280__)
#define MONITOR 1
#endif

#endif // MCU_CONFIG_H
