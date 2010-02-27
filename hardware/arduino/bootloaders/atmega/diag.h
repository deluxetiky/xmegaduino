#ifndef DIAG_H
#define DIAG_H

/*
    Diagnostic output while bootloading.
*/

#if !defined DIAG_ENABLE
    #define DIAG_ENABLE 0
#endif

// TODO: Move to diag.h
#if 1 <= DIAG_ENABLE
    static void DiagEnable(uint8_t bootuart);
    static void DiagChar(char value);
    static void Diag(const char *value);
    static void DiagNumber(unsigned long value, uint8_t base);
#else
    #define DiagEnable(bootuart)
#endif


#endif // DIAG_H
