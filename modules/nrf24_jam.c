#include "nrf24_jam.h"
#include "nrf24_spi.h"
#include <furi.h>

/* ------------------------------------------------------------------ */
/*  Channel tables                                                      */
/* ------------------------------------------------------------------ */

/* ALL: channels 0-82 stepped */
static const uint8_t ch_all[]      = {
     0,  2,  4,  6,  8, 10, 12, 14, 16, 18, 20, 22, 24, 26,
    28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54,
    56, 58, 60, 62, 64, 66, 68, 70, 72, 74, 76, 78, 80, 82
};

/* BT Classic: 79 channels 0-78 (1 MHz steps above 2402 MHz) */
static const uint8_t ch_bt[]       = {
     0,  5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 78
};

/* Zigbee: 802.15.4 channels 11-26 (center = 2405 + (ch-11)*5 MHz) */
static const uint8_t ch_zigbee[]   = {
     3,  8, 13, 18, 23, 28, 33, 38, 43, 48, 53, 58, 63, 68, 73, 78
};

/* WiFi ch 1 overlaps 2401-2423 MHz → NRF ch 0-22 */
static const uint8_t ch_wifi1[]    = {
     0,  2,  4,  6,  8, 10, 12, 14, 16, 18, 20, 22
};

/* WiFi ch 6 overlaps 2426-2448 MHz → NRF ch 24-46 */
static const uint8_t ch_wifi6[]    = {
    24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46
};

/* WiFi ch 11 overlaps 2451-2473 MHz → NRF ch 49-71 */
static const uint8_t ch_wifi11[]   = {
    49, 51, 53, 55, 57, 59, 61, 63, 65, 67, 69, 71
};

/* Logitech Unifying: known channels */
static const uint8_t ch_logitech[] = {
     5,  8, 11, 14, 17, 20, 23, 26, 29, 32, 35, 38, 41, 44,
    47, 50, 53, 56, 59, 62, 65, 68, 71, 74
};

typedef struct {
    const uint8_t* channels;
    uint8_t        count;
} ChannelSet;

static const ChannelSet channel_sets[] = {
    {ch_all,      sizeof(ch_all)      / sizeof(*ch_all)},
    {ch_bt,       sizeof(ch_bt)       / sizeof(*ch_bt)},
    {ch_zigbee,   sizeof(ch_zigbee)   / sizeof(*ch_zigbee)},
    {ch_wifi1,    sizeof(ch_wifi1)    / sizeof(*ch_wifi1)},
    {ch_wifi6,    sizeof(ch_wifi6)    / sizeof(*ch_wifi6)},
    {ch_wifi11,   sizeof(ch_wifi11)   / sizeof(*ch_wifi11)},
    {ch_logitech, sizeof(ch_logitech) / sizeof(*ch_logitech)},
};

/* ------------------------------------------------------------------ */
/*  State                                                               */
/* ------------------------------------------------------------------ */

typedef struct {
    bool           running;
    const uint8_t* channels;
    uint8_t        ch_count;
    uint8_t        ch_idx;
    uint8_t        current_ch;
    FuriTimer*     timer;
} Nrf24JamState;

static Nrf24JamState s = {0};

/* ------------------------------------------------------------------ */
/*  Timer callback — fired every 2 ms                                  */
/* ------------------------------------------------------------------ */

static void nrf24_jam_timer_cb(void* ctx) {
    (void)ctx;
    if(!s.running) return;

    s.current_ch = s.channels[s.ch_idx];
    s.ch_idx     = (s.ch_idx + 1) % s.ch_count;

    /* Set channel */
    nrf24_write_reg(NRF24_REG_RF_CH, s.current_ch);

    /* Flush old payload */
    nrf24_cmd(NRF24_CMD_FLUSH_TX);

    /* Write 32 bytes of noise */
    uint8_t noise[32];
    uint32_t rng = furi_get_tick();
    for(uint8_t i = 0; i < 32; i++) {
        rng ^= (rng << 13);
        rng ^= (rng >> 17);
        rng ^= (rng << 5);
        noise[i] = (uint8_t)(rng & 0xFF);
    }
    nrf24_write_tx_payload(noise, 32);

    /* Pulse CE to transmit */
    nrf24_pulse_ce();
}

/* ------------------------------------------------------------------ */
/*  Public API                                                          */
/* ------------------------------------------------------------------ */

void nrf24_jam_start(Nrf24JamMode mode, uint8_t power) {
    if(s.running) nrf24_jam_stop();

    const ChannelSet* cs = &channel_sets[(uint8_t)mode];
    s.channels   = cs->channels;
    s.ch_count   = cs->count;
    s.ch_idx     = 0;
    s.current_ch = cs->channels[0];
    s.running    = true;

    nrf24_spi_init();

    // Power-Level via RF_SETUP Bits 1-2:
    // 00 = -18 dBm, 01 = -12 dBm, 10 = -6 dBm, 11 = 0 dBm
    // RF_DR_HIGH (Bit 3) = 1 fuer 2 Mbps
    uint8_t pwr_bits = (power & 0x03) << 1;
    nrf24_write_reg(NRF24_REG_RF_SETUP, NRF24_RF_SETUP_2MBPS | pwr_bits);

    s.timer = furi_timer_alloc(nrf24_jam_timer_cb, FuriTimerTypePeriodic, NULL);
    furi_timer_start(s.timer, 2);
}

void nrf24_jam_stop(void) {
    if(!s.running) return;
    s.running = false;

    if(s.timer) {
        furi_timer_stop(s.timer);
        furi_timer_free(s.timer);
        s.timer = NULL;
    }

    nrf24_spi_deinit();
}

uint8_t nrf24_jam_current_channel(void) {
    return s.current_ch;
}
