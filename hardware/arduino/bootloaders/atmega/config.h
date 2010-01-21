#ifndef CONFIG_H
#define CONFIG_H

#include "mcu_config.h"
#include "board_config.h"

/** config.h defines defaults for various configuration macros.
  * These defaults may be overridden by mcu_config.h and board_config.h.
  *
  * mcu_config.h is for those macros which are tied to CPU.
  *
  * board_config.h is for those macros which are tied to
  * arduino board and variant.
**/

/* After this many errors give up and launch application */
#define MAX_ERROR_COUNT 5

/* SW_MAJOR and MINOR needs to be updated from time to time to avoid warning message from AVR Studio */
/* never allow AVR Studio to do an update !!!! */
#define HW_VER   0x02
#define SW_MAJOR 0x01
#define SW_MINOR 0x10

#if !defined USE_BUILT_IN_AVR_EEPROM_H 
    #define USE_BUILT_IN_AVR_EEPROM_H 1
#endif

/* the current avr-libc eeprom functions do not support the ATmega168 */
/* own eeprom write/read functions are used instead */
#if USE_BUILT_IN_AVR_EEPROM_H 
    #include <avr/eeprom.h>
#endif

/* Use the F_CPU defined in Makefile */

/* Set the waiting time for the bootloader */
/* Define if not defined by the Makefile or mcu_config.h */
#if ! defined MAX_TIME_COUNT 
    #define MAX_TIME_COUNT (F_CPU>>4)
#endif

#ifdef __AVR_ATmega128__
    #define START_APP_IF_FLASH_PROGRAMED 1
#endif

#if !defined START_APP_IF_FLASH_PROGRAMED
    #define START_APP_IF_FLASH_PROGRAMED 0
#endif

/* set the UART baud rate */
#if !defined BAUD_RATE
    #define BAUD_RATE   19200
#endif

/* UART names. Settle on UBRRnL convention, remapping those that use UBRRL. */
#if !defined UBRR0L && defined UBRRL
    #define UBRR0L UBRRL
    #define UBRR0H UBRRH  
    #define UCSR0B UCSRB
    #define UCSR0C UCSRC
    #define TXEN0  TXEN
    #define RXEN0  RXEN
#endif
#if !defined UCSR0A && defined UCSRA
    #define UCSR0A UCSRA
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

#if !defined BL0
/* Bootloader pin. */
#define BL_DDR  DDRD
#define BL_PORT PORTD
#define BL_PIN  PIND
#define BL0     PIND6
#endif

/* set pin direction for bootloader pin and enable pullup */
/* for ATmega128, two pins need to be initialized */
#ifdef __AVR_ATmega128__
    #define INIT_BL0_DIRECTION 1
    #define INIT_BL1_DIRECTION 1
#endif
#if !defined(INIT_BL0_DIRECTION )
    /* We run the bootloader regardless of the state of this pin.  Thus, don't
    put it in a different state than the other pins.  --DAM, 070709
    This also applies to Arduino Mega -- DC, 080930
    */
    #define INIT_BL0_DIRECTION 0
#endif
#if !defined(INIT_BL1_DIRECTION )
    #define INIT_BL1_DIRECTION 0
#endif

#if defined __AVR_ATmega128__ || defined __AVR_ATmega1280__
    #define LED_FLASHES_AT_BOOT NUM_LED_FLASHES + bootuart
#endif

#if !defined LED_FLASHES_AT_BOOT
    #define LED_FLASHES_AT_BOOT NUM_LED_FLASHES
#endif

/* onboard LED is used to indicate, that the bootloader was entered (3x flashing) */
/* if monitor functions are included, LED goes on after monitor was entered */

#if !defined LED_PORT
    #define LED_DDR  DDRB
    #define LED_PORT PORTB
    #define LED_PIN  PINB
#endif

/* Onboard LED is connected to pin PB5 in Arduino NG, Diecimila, and Duomilanuove */ 
/* other boards like e.g. Crumb8, Crumb168 are using PB2 */
#if !defined LED
    #define LED      PINB5
#endif

/* SPM control and status register name. Settle on SPMCSR and remap those using SPMCR. */
#if !defined SPMCSR
    #define SPMCSR SPMCR
#endif

#if !defined SPM_PRE
    #define SPM_PRE
#endif

#if !defined SPM_POST
    #define SPM_POST
#endif

#if !defined EEWE && defined EEPE
    #define EEWE EEPE
#endif

#endif // CONFIG_H
