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
        "lds   r16,%[SPM_CSR]      \n\t" //Wait for previous spm to complete
        "andi  r16,1               \n\t"
        "cpi   r16,1               \n\t"
        "breq  wait_spm            \n\t"

        SPM_PRE
        "sts   %[SPM_CSR],%[code]  \n\t"
        "spm                       \n\t"
        SPM_POST
        : [SPM_CSR] "=m" (SPMCSR)
        : [code] "r" (code), [addr] "r" (addr), [value] "r" (value)
        : "r0", "r16", "r30", "r31"
    );
}
/* For Xmega
sts NVM_CMD, r20     ; Load prepared command into NVM Command register.
ldi r18, CCP_SPM_gc  ; Prepare Protect SPM signature in R18
sts CCP, r18         ; Enable SPM operation (this disables interrupts for 4 cycles).
spm                      ; Self-program.

SP_WaitForSPM:
lds r18, NVM_STATUS     ; Load the NVM Status register.
sbrc    r18, NVM_NVMBUSY_bp ; Check if bit is cleared.
rjmp    SP_WaitForSPM       ; Repeat check if bit is not cleared.
clr r18
sts NVM_CMD, r18        ; Clear up command register to NO_OPERATION.
ret

*/
