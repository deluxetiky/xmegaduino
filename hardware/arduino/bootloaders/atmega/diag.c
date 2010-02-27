/* some includes */
#if 0 // TODO: Kill this
#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>
#endif

#include "config.h"
#include "diag.h"

#if 1 <= DIAG_ENABLE
    USART_t* usart_diag;


    void DiagEnable(uint8_t bootuart)
    {
        // TODO: Which uart is used for diag should be spec'd in board.h
        if (2 == bootuart) {
            usart_diag = &USARTD0;
        } else {
            usart_diag = &USARTD1;
        }
    }

    void DiagChar(char c)
    {
      while ( !USART_IS_TX_READY(*usart_diag) );
      USART_PUT_CHAR(*usart_diag, c);
    }

    void Diag(const char *str)
    {
      while (*str)
        DiagChar(*str++);
    }

    void DiagNumber(unsigned long n, uint8_t base)
    {
      unsigned char buf[8 * sizeof(long)]; // Assumes 8-bit chars. 
      unsigned long i = 0;

      if (n == 0) {
        Diag("0");
        return;
      } 

      while (n > 0) {
        buf[i++] = n % base;
        n /= base;
      }

      for (; i > 0; i--)
        DiagChar((char) (buf[i - 1] < 10 ?
          '0' + buf[i - 1] :
          'A' + buf[i - 1] - 10));
    }

#endif

/* end of file ATmegaBOOT.c */
