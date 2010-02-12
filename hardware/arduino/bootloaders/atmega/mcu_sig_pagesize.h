#ifndef MCU_SIG_PAGESIZE_H
#define MCU_SIG_PAGESIZE_H

/*
MCU signature bytes, page size, and number of bits required to address all code.
For example, an atmega8 has 8K of code and requires 13 address bits.
An atmega32 has 32K of code, and requires 15 address bits.
An xmega128a1 with 128K+8K of code, or 136K bytes, requires 18 address bits.
*/

/* manufacturer byte is always the same */
// Yep, Atmel is the only manufacturer of AVR micros.  Single source :(
#define SIG1    0x1E    

#if defined __AVR_ATxmega128A1__
#define SIG2    0x97
#define SIG3    0x4C
#define PAGE_SIZE   0x100U   //256 words
#define ADDR_BITS   18

#elif defined __AVR_ATmega1280__
#define SIG2    0x97
#define SIG3    0x03
#define PAGE_SIZE   0x80U   //128 words
#define ADDR_BITS   17

#elif defined __AVR_ATmega1281__
#define SIG2    0x97
#define SIG3    0x04
#define PAGE_SIZE   0x80U   //128 words
#define ADDR_BITS   17

#elif defined __AVR_ATmega128__
#define SIG2    0x97
#define SIG3    0x02
#define PAGE_SIZE   0x80U   //128 words
#define ADDR_BITS   17

#elif defined __AVR_ATmega644P__
#define SIG2    0x96
#define SIG3    0x0A
#define PAGE_SIZE   0x80U   //128 words
#define ADDR_BITS   16

#elif defined __AVR_ATmega644__
#define SIG2    0x96
#define SIG3    0x09
#define PAGE_SIZE   0x80U   //128 words
#define ADDR_BITS   16

#elif defined __AVR_ATmega324P__
#define SIG2    0x95
#define SIG3    0x08
#define PAGE_SIZE   0x80U   //128 words
#define ADDR_BITS   15

#elif defined __AVR_ATmega64__
#define SIG2    0x96
#define SIG3    0x02
#define PAGE_SIZE   0x80U   //128 words
#define ADDR_BITS   16

#elif defined __AVR_ATmega32__
#define SIG2    0x95
#define SIG3    0x02
#define PAGE_SIZE   0x40U   //64 words
#define ADDR_BITS   15

#elif defined __AVR_ATmega16__
#define SIG2    0x94
#define SIG3    0x03
#define PAGE_SIZE   0x40U   //64 words
#define ADDR_BITS   14

#elif defined __AVR_ATmega8__
#define SIG2    0x93
#define SIG3    0x07
#define PAGE_SIZE   0x20U   //32 words
#define ADDR_BITS   13

#elif defined __AVR_ATmega88__
#define SIG2    0x93
#define SIG3    0x0a
#define PAGE_SIZE   0x20U   //32 words
#define ADDR_BITS   13

#elif defined __AVR_ATmega168__
#define SIG2    0x94
#define SIG3    0x06
#define PAGE_SIZE   0x40U   //64 words
#define ADDR_BITS   14

#elif defined __AVR_ATmega328P__
#define SIG2    0x95
#define SIG3    0x0F
#define PAGE_SIZE   0x40U   //64 words
#define ADDR_BITS   15

#elif defined __AVR_ATmega162__
#define SIG2    0x94
#define SIG3    0x04
#define PAGE_SIZE   0x40U   //64 words
#define ADDR_BITS   14

#elif defined __AVR_ATmega163__
#define SIG2    0x94
#define SIG3    0x02
#define PAGE_SIZE   0x40U   //64 words
#define ADDR_BITS   14

#elif defined __AVR_ATmega169__
#define SIG2    0x94
#define SIG3    0x05
#define PAGE_SIZE   0x40U   //64 words
#define ADDR_BITS   14

#elif defined __AVR_ATmega8515__
#define SIG2    0x93
#define SIG3    0x06
#define PAGE_SIZE   0x20U   //32 words
#define ADDR_BITS   13

#elif defined __AVR_ATmega8535__
#define SIG2    0x93
#define SIG3    0x08
#define PAGE_SIZE   0x20U   //32 words
#define ADDR_BITS   13
#endif

#endif // MCU_SIG_PAGESIZE_H
