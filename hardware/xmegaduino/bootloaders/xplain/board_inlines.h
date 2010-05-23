#ifndef BOARD_INLINES_H
#define BOARD_INLINES_H

void app_start(void); // TODO: Move this to a #include

#define SetBootloaderPinDirections()

static inline uint8_t GetBootUart() {
    /* Only one uart: 0 */
    return 0;
}

static inline void InitBootUart(int bootuart) {
    USART0_SET_DIR();
    USART0_SET_BAUD(BAUD_RATE);
    USART0_RX_ENABLE();
    USART0_TX_ENABLE();
    USART0_SET_TO_8N1();
}

static inline void SuppressLineNoise() {
    #if LINE_NOISE_PIN
        /* Enable internal pull-up resistor on pin D0 (RX), in order
        to supress line noise that prevents the bootloader from
        timing out (DAM: 20070509) */
        DDR_LINE_NOISE  &= ~_BV(LINE_NOISE_PIN);
        PORT_LINE_NOISE |= _BV(LINE_NOISE_PIN);
    #endif
}

#endif // BOARD_INLINES_H
