#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef enum {
    Nrf24JamAll      = 0,
    Nrf24JamBt,
    Nrf24JamZigbee,
    Nrf24JamWifi1,
    Nrf24JamWifi6,
    Nrf24JamWifi11,
    Nrf24JamLogitech,
} Nrf24JamMode;

/** Initialise NRF24 and start hopping on the channel set for mode. */
void nrf24_jam_start(Nrf24JamMode mode);

/** Stop jamming and release SPI / GPIO. */
void nrf24_jam_stop(void);

/** Returns the RF channel that was last written. */
uint8_t nrf24_jam_current_channel(void);
