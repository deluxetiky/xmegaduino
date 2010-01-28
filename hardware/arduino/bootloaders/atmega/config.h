#ifndef CONFIG_H
#define CONFIG_H

#include "mcu.h"
#include "mcu_config.h"
#include "mcu_defaults.h"
#include "board_config.h"

/** config.h defines defaults for various configuration/policy macros.
  * These should be relatively arbitrary policies not dictated by
  * the MCU. Strongly influenced, perhaps, but not determined by.
  * For example, program buffer size, but not PAGE_SIZE.
  *
  * These defaults may be overridden by mcu_config.h and board_config.h.
  *
  * mcu_config.h is for those macros which are tied to CPU.
  *
  * board_config.h is for those macros which are tied to
  * arduino board and variant.
**/

/* SW_MAJOR and MINOR needs to be updated from time to time to avoid warning message from AVR Studio */
/* never allow AVR Studio to do an update !!!! */
#define HW_VER   0x02
#define SW_MAJOR 0x01
#define SW_MINOR 0x10

/* After this many errors give up and launch application */
#define MAX_ERROR_COUNT 5

#if PAGE_SIZE <= 128
   #define LOADER_BUFF_SIZE 256
#else
   #define LOADER_BUFF_SIZE PAGE_SIZE * 2
#endif

/* Set the waiting time for the bootloader */
/* Define if not defined by the Makefile or mcu_config.h */
#if ! defined MAX_TIME_COUNT 
    #define MAX_TIME_COUNT (F_CPU>>4)
#endif

#if !defined START_APP_IF_FLASH_PROGRAMED
    #define START_APP_IF_FLASH_PROGRAMED 0
#endif

/* set the UART baud rate */
#if !defined BAUD_RATE
    #define BAUD_RATE   19200
#endif

#if !defined LED_FLASHES_AT_BOOT
    #define LED_FLASHES_AT_BOOT NUM_LED_FLASHES
#endif

#endif // CONFIG_H
