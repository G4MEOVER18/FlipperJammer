#include "ble_jam.h"
#include "nrf24_spi.h"
#include <furi.h>
#include <furi_hal_power.h>

static bool s_otg_by_ble = false;

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

    /* Const Carrier hop: CE low -> RF_CH -> CE high */
    nrf24_ce_set(false);
    nrf24_write_reg(NRF24_REG_RF_CH, ch);
    nrf24_ce_set(true);
}

void ble_jam_start(void) {
    if(s.running) ble_jam_stop();

    s.running = true;
    s.ch_idx  = 0;

    /* OTG-5V einschalten — E01C-ML01SP4 und PA+LNA Module brauchen mehr Power */
    if(!furi_hal_power_is_otg_enabled()) {
        furi_hal_power_enable_otg();
        furi_delay_ms(20);
        s_otg_by_ble = true;
    }

    nrf24_spi_init();

    /* Continuous Carrier Mode, max power (+0 dBm) */
    nrf24_write_reg(NRF24_REG_RF_SETUP,
        NRF24_RF_SETUP_CONT_WAVE | NRF24_RF_SETUP_PLL_LOCK |
        NRF24_RF_SETUP_2MBPS | NRF24_RF_SETUP_PWR_MAX);

    nrf24_write_reg(NRF24_REG_RF_CH, ble_channels[0]);
    furi_delay_us(140);
    nrf24_ce_set(true);  // LED leuchtet dauerhaft

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

    nrf24_ce_set(false);
    nrf24_write_reg(NRF24_REG_RF_SETUP, NRF24_RF_SETUP_2MBPS);
    nrf24_spi_deinit();

    if(s_otg_by_ble && furi_hal_power_is_otg_enabled()) {
        furi_hal_power_disable_otg();
        s_otg_by_ble = false;
    }
}
