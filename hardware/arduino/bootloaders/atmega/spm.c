/* $Id$ */

/* some includes */
#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include "config.h"

extern void Spm(uint8_t code, uint16_t addr, uint16_t value) {
    asm volatile(
        "movw  r30,%[addr]         \n\t" // Load Z
        "movw  r0,%[value]         \n\t" // Load value. Not used by all ops.

        "wait_spm:                 \n\t"
        "lds   r16,%[SPM_SREG]     \n\t" //Wait for previous spm to complete
        "andi  r16,1               \n\t"
        "cpi   r16,1               \n\t"
        "breq  wait_spm            \n\t"
        "sts   %[SPM_SREG],%[code] \n\t"
        "spm                       \n\t"
#ifdef __AVR_ATmega163__
        ".word 0xFFFF           \n\t"
        "nop                    \n\t"
#endif
        : [SPM_SREG] "=m" (SPM_STATUS_REG)
        : [code] "r" (code), [addr] "r" (addr), [value] "r" (value)
        : "r0", "r16", "r30", "r31"
    );
}
