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
        "lds    r16,%[spm_status]  \n\t"
#if 0
        // TODO: Use better wait_spm loop.
        // This should work, and is one instruction shorter. But it doesn't work,
        // so comment out and use slightly slower loop.
        "sbrc   r16,%[spm_busy]    \n\t"
        "rjmp   wait_spm           \n\t"
#else
        "andi  r16,%[spm_busy]     \n\t"
        "cpi   r16,%[spm_busy]     \n\t"
        "breq  wait_spm            \n\t"
#endif

        "sts   %[spm_cmd],%[code]  \n\t"
#if defined CCP
        ldi   r16, CCP_SPM_gc
        sts   CCP, r16
#endif
        "spm                       \n\t"
        SPM_POST
        // TODO: Load noop into SPM_CMD
        :
        : [spm_cmd]    "m"  (SPM_CMD),
          [spm_status] "m"  (SPM_STATUS),
          [spm_busy]   "i"  (SPM_BUSY),
          [code]       "r"  (code),
          [addr]       "r"  (addr),
          [value]      "r"  (value)
        : "r0", "r16", "r30", "r31"
    );
}
