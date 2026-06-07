#include "ble_jam.h"
#include "nrf24_spi.h"
#include <furi.h>

/* BLE advertising channels mapped to NRF24 RF_CH register values */
static const uint8_t ble_channels[] = {2, 26, 80};

typedef struct {
    bool       running;
    uint8_t    ch_idx;
    FuriTimer* timer;
} BleJamState;

static BleJamState s = {0};

static void ble_jam_timer_cb(void* ctx) {
    (void)ctx;
    if(!s.running) return;

    uint8_t ch = ble_channels[s.ch_idx];
    s.ch_idx   = (s.ch_idx + 1) % 3;

    nrf24_write_reg(NRF24_REG_RF_CH, ch);
    nrf24_cmd(NRF24_CMD_FLUSH_TX);

    /* 32-byte noise payload */
    uint8_t noise[32];
    uint32_t rng = furi_get_tick() ^ 0xBEEF0000UL;
    for(uint8_t i = 0; i < 32; i++) {
        rng ^= rng << 13;
        rng ^= rng >> 17;
        rng ^= rng << 5;
        noise[i] = (uint8_t)rng;
    }
    nrf24_write_tx_payload(noise, 32);
    nrf24_pulse_ce();
}

void ble_jam_start(void) {
    if(s.running) ble_jam_stop();

    s.running = true;
    s.ch_idx  = 0;

    nrf24_spi_init();

    /* 2 Mbps, max power — same as nrf24_jam but narrowband enough for BLE */
    nrf24_write_reg(NRF24_REG_RF_SETUP, 0x0E); /* 2 Mbps, +0 dBm */

    s.timer = furi_timer_alloc(ble_jam_timer_cb, FuriTimerTypePeriodic, NULL);
    furi_timer_start(s.timer, 1); /* 1 ms per channel */
}

void ble_jam_stop(void) {
    if(!s.running) return;
    s.running = false;

    if(s.timer) {
        furi_timer_stop(s.timer);
        furi_timer_free(s.timer);
        s.timer = NULL;
    }

    nrf24_spi_deinit();
}
