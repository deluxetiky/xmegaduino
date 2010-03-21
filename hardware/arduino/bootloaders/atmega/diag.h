#ifndef DIAG_H
#define DIAG_H

/*
    Diagnostic output while bootloading.
*/

#if !defined DIAG_ENABLE
    #define DIAG_ENABLE 1
#endif

// TODO: Move to diag.h
#if 1 <= DIAG_ENABLE
    void DiagEnable(uint8_t bootuart);
    void DiagChar(char value);
    void Diag(const char *value);
    void DiagNumber(unsigned long value, uint8_t base);
	void DiagStrNumber( const char* str, unsigned long n, uint8_t base, int nl );
#else
    #define DiagEnable(bootuart)
	#define DiagChar(value)
	#define Diag(value)
	#define DiagNumber(value, base)
	#define DiagStrNumber(str, number, base, nl)
#endif


#endif // DIAG_H
