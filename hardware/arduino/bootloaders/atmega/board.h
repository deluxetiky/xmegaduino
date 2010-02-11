#ifndef BOARD_H
#define BOARD_H

#if mega == TARGET                \
        || CRUMB128     == TARGET \
        || PROBOMEGA128 == TARGET \
        || SAVVY128     == TARGET
    #define LED      PINB7
#elif sanguino == TARGET
    #define LED      PINB0
#elif xplain == TARGET
    // PORTE
    #define LED      PINB0
    #define LED_DDR  DDRB
    #define LED_PORT PORTB
    #define LED_PIN  PINB
#endif

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
