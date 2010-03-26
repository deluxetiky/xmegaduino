/* some includes */
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

    void DiagStrNumber( const char* str, unsigned long n, uint8_t base, int nl )
	{
		Diag(str);
		DiagNumber(n, base);
		if (nl) {
		    Diag("\n");
		}
	}

#endif

/* end of file ATmegaBOOT.c */
