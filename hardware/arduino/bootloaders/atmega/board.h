#ifndef BOARD_H
#define BOARD_H

// USARTs

#if xplain == TARGET
    #define BL_PORT      PORTF
    #define BL_DIR       BL_PORT.DIR
    #define BL_IN        BL_PORT.IN
    #define BL_OUT       BL_PORT.OUT

    #define BL_0_PIN     0
    #define BL_1_PIN     1
    #define BL_2_PIN     2
    #define APP_PIN      3

    #define USART_0        USARTC0
    #define USART_0_PORT   PORTC
    #define USART_0_RD_PIN 2
    #define USART_0_WR_PIN 3
    #define BAUD_RATE_0    9600UL

    #define USART_1        USARTD0
    #define USART_1_PORT   PORTD
    #define USART_1_RD_PIN 2
    #define USART_1_WR_PIN 3
    #define BAUD_RATE_1    57600UL

    #define USART_2        USARTD1
    #define USART_2_PORT   PORTD
    #define USART_2_RD_PIN 6
    #define USART_2_WR_PIN 7
    #define BAUD_RATE_2    57600UL
#endif


// LED

#if mega == TARGET                \
        || CRUMB128     == TARGET \
        || PROBOMEGA128 == TARGET \
        || SAVVY128     == TARGET
    #define LED      PINB7
#elif sanguino == TARGET
    #define LED      PINB0
#elif xplain == TARGET
    // PORTE
    #define LED_PORT PORTE
    #define LED_DDR  LED_PORT.DIR
    #define LED_OUT  PORTE.OUT
#endif

// MONITOR

#if mega == TARGET
    #define MONITOR_WELCOME "ATmegaBOOT / Arduino Mega - (C) Arduino LLC - 090930\n\r";
#elif CRUMB128 == TARGET
    #define MONITOR_WELCOME "ATmegaBOOT / Crumb128 - (C) J.P.Kyle, E.Lins - 050815\n\r";
#elif PROBOMEGA128 == TARGET
    #define MONITOR_WELCOME "ATmegaBOOT / PROBOmega128 - (C) J.P.Kyle, E.Lins - 050815\n\r";
#elif SAVVY128 == TARGET
    #define MONITOR_WELCOME "ATmegaBOOT / Savvy128 - (C) J.P.Kyle, E.Lins - 050815\n\r";
#else
    #define MONITOR_WELCOME "ATmegaBOOT / Unknown\n\r";
#endif

#endif // BOARD_H
