#include "subghz_jam.h"
#include <furi.h>
#include <furi_hal.h>
#include <lib/subghz/devices/devices.h>
#include <applications/drivers/subghz/cc1101_ext/cc1101_ext_interconnect.h>
#include <lib/subghz/devices/cc1101_int/cc1101_int_interconnect.h>
#include <lib/toolbox/level_duration.h>
#include <furi_hal_power.h>

/* ------------------------------------------------------------------ */
/*  State                                                               */
/* ------------------------------------------------------------------ */

typedef enum { SRF_IDLE, SRF_TX, SRF_ASYNC_TX } SubGhzRfOp;

typedef struct {
    bool              running;
    SubGhzJamMode     mode;
    float             base_freq_mhz;
    float             sweep_freq_mhz;
    FuriTimer*        timer;
    const SubGhzDevice* device;
    SubGhzRfOp        rf_op;
} SubGhzJamState;

static SubGhzJamState s = {0};
static bool s_otg_by_jam = false;

/* ------------------------------------------------------------------ */
/*  OTG power                                                           */
/* ------------------------------------------------------------------ */

static void jam_power_on(void) {
    uint8_t attempts = 0;
    while(!furi_hal_power_is_otg_enabled() && attempts++ < 5) {
        furi_hal_power_enable_otg();
        furi_delay_ms(10);
    }
    if(furi_hal_power_is_otg_enabled()) s_otg_by_jam = true;
}

static void jam_power_off(void) {
    if(s_otg_by_jam && furi_hal_power_is_otg_enabled()) {
        furi_hal_power_disable_otg();
        s_otg_by_jam = false;
    }
}

/* ------------------------------------------------------------------ */
/*  TX Callbacks                                                        */
/* ------------------------------------------------------------------ */

// Noise: 250µs alternating OOK-Pulse (korrekte LevelDuration-Signatur!)
static LevelDuration noise_tx_cb(void* ctx) {
    (void)ctx;
    static bool lvl = false;
    lvl = !lvl;
    return level_duration_make(lvl, 250);
}

/* ------------------------------------------------------------------ */
/*  Timer callbacks                                                     */
/* ------------------------------------------------------------------ */

static void sweep_timer_cb(void* ctx) {
    (void)ctx;
    if(!s.running || !s.device) return;

    s.sweep_freq_mhz += 0.1f;
    if(s.sweep_freq_mhz > s.base_freq_mhz + 4.0f)
        s.sweep_freq_mhz = s.base_freq_mhz - 2.0f;

    subghz_devices_idle(s.device);
    subghz_devices_set_frequency(s.device, (uint32_t)(s.sweep_freq_mhz * 1000000.0f));
    subghz_devices_set_tx(s.device);
    s.rf_op = SRF_TX;
}

/* ------------------------------------------------------------------ */
/*  Device init/deinit helper                                           */
/* ------------------------------------------------------------------ */

static bool jam_device_init(bool external) {
    subghz_devices_init();

    if(external) {
        jam_power_on();
        const SubGhzDevice* ext = subghz_devices_get_by_name(SUBGHZ_DEVICE_CC1101_EXT_NAME);
        if(ext && subghz_devices_is_connect(ext)) {
            subghz_devices_begin(ext);
            s.device = ext;
            return true;
        }
        jam_power_off();
    }

    const SubGhzDevice* dev = subghz_devices_get_by_name(SUBGHZ_DEVICE_CC1101_INT_NAME);
    if(!dev) { subghz_devices_deinit(); return false; }
    subghz_devices_begin(dev); // begin=NULL für int → no-op
    s.device = dev;
    return true;
}

static void jam_device_deinit(void) {
    if(s.device) {
        if(s.rf_op == SRF_ASYNC_TX)
            subghz_devices_stop_async_tx(s.device);
        else if(s.rf_op == SRF_TX || s.rf_op == SRF_IDLE)
            subghz_devices_idle(s.device);
        subghz_devices_sleep(s.device);
        subghz_devices_end(s.device);
        s.device = NULL;
    }
    jam_power_off();
    subghz_devices_deinit();
    s.rf_op = SRF_IDLE;
}

/* ------------------------------------------------------------------ */
/*  Public API                                                          */
/* ------------------------------------------------------------------ */

void subghz_jam_start(float freq_mhz, SubGhzJamMode mode, bool external) {
    if(s.running) subghz_jam_stop();

    s.mode           = mode;
    s.base_freq_mhz  = freq_mhz;
    s.sweep_freq_mhz = freq_mhz - 2.0f;
    s.rf_op          = SRF_IDLE;

    if(!jam_device_init(external)) return;

    uint32_t freq_hz = (uint32_t)(freq_mhz * 1000000.0f);
    subghz_devices_reset(s.device);
    subghz_devices_load_preset(s.device, FuriHalSubGhzPresetOok650Async, NULL);
    subghz_devices_set_frequency(s.device, freq_hz);

    s.running = true;

    switch(mode) {
    case SubGhzJamCarrier:
        subghz_devices_set_tx(s.device);
        s.rf_op = SRF_TX;
        break;

    case SubGhzJamNoise:
        subghz_devices_start_async_tx(s.device, noise_tx_cb, NULL);
        s.rf_op = SRF_ASYNC_TX;
        break;

    case SubGhzJamSweep:
        subghz_devices_set_tx(s.device);
        s.rf_op = SRF_TX;
        s.timer = furi_timer_alloc(sweep_timer_cb, FuriTimerTypePeriodic, NULL);
        furi_timer_start(s.timer, 50);
        break;
    }
}

void subghz_jam_stop(void) {
    if(!s.running) return;
    s.running = false;

    // Timer ZUERST stoppen — verhindert Race-Condition mit async-Operationen
    if(s.timer) {
        furi_timer_stop(s.timer);
        furi_timer_free(s.timer);
        s.timer = NULL;
    }

    jam_device_deinit();
}

bool subghz_jam_is_running(void) {
    return s.running;
}
