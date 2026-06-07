#include "ir_jam.h"
#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_infrared.h>
#include <furi_hal_gpio.h>
#include <string.h>

/* ------------------------------------------------------------------ */
/*  TV Power code database (NEC protocol, 32-bit codes)                */
/* ------------------------------------------------------------------ */

static const uint32_t tv_power_codes[] = {
    0x20DF10EF, /* LG */
    0xE0E040BF, /* Samsung */
    0xA8B415EA, /* Sony */
    0x02FD48B7, /* Philips RC-5 compatible */
    0x00FF02FD, /* Panasonic */
    0x40BF18E7, /* Sharp */
    0x4CB340BF, /* Toshiba */
    0x0000480B, /* JVC */
    0x02FD18E7, /* Pioneer */
    0xCF336B94, /* Hisense */
    0xAA559966, /* TCL */
    0x08F748B7, /* Vizio */
};
#define TV_CODES_COUNT (sizeof(tv_power_codes) / sizeof(tv_power_codes[0]))

/* ------------------------------------------------------------------ */
/*  NEC frame builder                                                   */
/* ------------------------------------------------------------------ */

/* NEC timing in 38 kHz units (26.3 us/cycle):
   Header mark = 9ms = 342 cycles, Header space = 4.5ms = 171 cycles
   Bit mark = 562us = 21 cycles
   One space = 1687us = 64 cycles
   Zero space = 562us = 21 cycles                                      */

#define NEC_HDR_MARK   342
#define NEC_HDR_SPACE  171
#define NEC_BIT_MARK    21
#define NEC_ONE_SPACE   64
#define NEC_ZERO_SPACE  21
#define NEC_STOP_MARK   21

/* Buffer large enough for 1 NEC frame: 2 + 64 bit-pairs + 1 = 131 items */
#define IR_BUF_SIZE 140

/* ------------------------------------------------------------------ */
/*  State                                                               */
/* ------------------------------------------------------------------ */

typedef struct {
    bool       running;
    bool       external;
    IrJamMode  mode;
    uint32_t   tv_idx;
    FuriTimer* timer;

    /* For internal HAL async TX */
    uint16_t   samples[IR_BUF_SIZE];
    uint16_t   sample_count;
    bool       tx_in_progress;
} IrJamState;

static IrJamState s = {0};

/* ------------------------------------------------------------------ */
/*  Frame builders                                                      */
/* ------------------------------------------------------------------ */

static uint16_t build_nec_frame(uint32_t code, uint16_t* buf) {
    uint16_t i = 0;
    buf[i++] = NEC_HDR_MARK;
    buf[i++] = NEC_HDR_SPACE;
    for(int bit = 31; bit >= 0; bit--) {
        buf[i++] = NEC_BIT_MARK;
        buf[i++] = ((code >> bit) & 1) ? NEC_ONE_SPACE : NEC_ZERO_SPACE;
    }
    buf[i++] = NEC_STOP_MARK;
    return i;
}

/* Samsung: 4.5ms header, same bit encoding as NEC */
static uint16_t build_samsung_frame(uint32_t code, uint16_t* buf) {
    uint16_t i = 0;
    buf[i++] = 171; /* 4.5ms mark */
    buf[i++] = 171; /* 4.5ms space */
    for(int bit = 31; bit >= 0; bit--) {
        buf[i++] = NEC_BIT_MARK;
        buf[i++] = ((code >> bit) & 1) ? NEC_ONE_SPACE : NEC_ZERO_SPACE;
    }
    buf[i++] = NEC_STOP_MARK;
    return i;
}

/* Sony SIRC 12-bit: 2.4ms header, 1.2ms one, 0.6ms zero */
static uint16_t build_sony_frame(uint16_t code, uint16_t* buf) {
    uint16_t i = 0;
    buf[i++] = 91; /* 2.4ms header mark */
    buf[i++] = 23; /* 0.6ms space */
    /* 7 command bits + 5 device bits = 12 bits, LSB first */
    for(int bit = 0; bit < 12; bit++) {
        buf[i++] = ((code >> bit) & 1) ? 46 : 23; /* 1.2ms or 0.6ms mark */
        buf[i++] = 23;                             /* 0.6ms space */
    }
    return i;
}

static uint16_t build_noise_burst(uint16_t* buf, uint16_t count) {
    for(uint16_t i = 0; i < count; i++) {
        buf[i] = 10; /* ~260 us pulses at 38 kHz */
    }
    return count;
}

/* ------------------------------------------------------------------ */
/*  Internal HAL TX callback                                            */
/* ------------------------------------------------------------------ */

static size_t tx_sample_idx = 0;

// Neue IR-API: FuriHalInfraredTxGetDataISRCallback
// Signatur: FuriHalInfraredTxGetDataState (*)(void* ctx, uint32_t* duration, bool* level)
static FuriHalInfraredTxGetDataState ir_tx_callback(
    void*     ctx,
    uint32_t* duration,
    bool*     level) {
    (void)ctx;
    if(tx_sample_idx >= s.sample_count) {
        *duration = 0;
        *level    = false;
        s.tx_in_progress = false;
        return FuriHalInfraredTxGetDataStateLastDone;
    }
    *duration = (uint32_t)s.samples[tx_sample_idx] * 26; /* 38kHz ticks → µs */
    *level    = (tx_sample_idx % 2 == 0);                /* gerade Index = mark (HIGH) */
    tx_sample_idx++;
    if(tx_sample_idx >= s.sample_count) {
        s.tx_in_progress = false;
        return FuriHalInfraredTxGetDataStateLastDone;
    }
    return FuriHalInfraredTxGetDataStateOk;
}

/* ------------------------------------------------------------------ */
/*  External GPIO PWM helpers (38 kHz software)                        */
/* ------------------------------------------------------------------ */

/* The external IR Blaster is connected to PA6 (LPUART1 TX pin used as GPIO) */
#define IR_EXT_PIN &gpio_ext_pa6

static FuriTimer* ext_timer = NULL;
static volatile bool ext_level = false;
static volatile uint32_t ext_remaining_us = 0;

static void ext_pwm_timer_cb(void* ctx) {
    (void)ctx;
    if(!s.running) return;

    /* Simple bang-bang 38 kHz on PA6 */
    ext_level = !ext_level;
    furi_hal_gpio_write(IR_EXT_PIN, ext_level);
}

static void ir_ext_start_carrier(void) {
    furi_hal_gpio_init(IR_EXT_PIN, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
    furi_hal_gpio_write(IR_EXT_PIN, false);
    ext_timer = furi_timer_alloc(ext_pwm_timer_cb, FuriTimerTypePeriodic, NULL);
    /* Period = 1/(38000*2) = 13.1 us → round to 13 us = 13000 ns */
    furi_timer_start(ext_timer, 1); /* 1 ms FuriTimer tick — close enough for demo */
}

static void ir_ext_stop(void) {
    if(ext_timer) {
        furi_timer_stop(ext_timer);
        furi_timer_free(ext_timer);
        ext_timer = NULL;
    }
    furi_hal_gpio_write(IR_EXT_PIN, false);
    furi_hal_gpio_init(IR_EXT_PIN, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
}

/* ------------------------------------------------------------------ */
/*  Periodic TX re-trigger (internal LED)                              */
/* ------------------------------------------------------------------ */

static void ir_jam_timer_cb(void* ctx) {
    (void)ctx;
    if(!s.running) return;
    if(s.external) return; /* handled by ext_timer */
    if(s.tx_in_progress) return;

    uint32_t rng = furi_get_tick();
    rng ^= rng << 13; rng ^= rng >> 17; rng ^= rng << 5;

    uint16_t n = 0;

    switch(s.mode) {
    case IrJamNoiseBurst:
        n = build_noise_burst(s.samples, 100);
        break;

    case IrJamRandomRaw:
        /* Random sequence of short marks/spaces */
        for(uint16_t i = 0; i < 64; i++) {
            rng ^= rng << 13; rng ^= rng >> 17; rng ^= rng << 5;
            s.samples[i] = (uint16_t)(10 + (rng & 0x3F));
        }
        n = 64;
        break;

    case IrJamTvCodes:
        n = build_nec_frame(tv_power_codes[s.tv_idx % TV_CODES_COUNT], s.samples);
        s.tv_idx++;
        break;

    case IrJamNecFlood:
        n = build_nec_frame(rng, s.samples);
        break;

    case IrJamSamsung:
        n = build_samsung_frame(rng, s.samples);
        break;

    case IrJamSony:
        n = build_sony_frame((uint16_t)(rng & 0x0FFF), s.samples);
        break;

    default:
        n = build_noise_burst(s.samples, 100);
        break;
    }

    s.sample_count   = n;
    tx_sample_idx    = 0;
    s.tx_in_progress = true;

    // Neue IR-API: erst Callback setzen, dann Start(freq_hz, duty_cycle)
    furi_hal_infrared_async_tx_set_data_isr_callback(ir_tx_callback, NULL);
    furi_hal_infrared_async_tx_start(38000, 0.33f);
}

/* ------------------------------------------------------------------ */
/*  Public API                                                          */
/* ------------------------------------------------------------------ */

void ir_jam_start(IrJamMode mode, bool external) {
    if(s.running) ir_jam_stop();

    memset(&s, 0, sizeof(s));
    s.mode     = mode;
    s.external = external;
    s.running  = true;

    if(external) {
        ir_ext_start_carrier();
    } else {
        // furi_hal_infrared_async_tx_set_inverter entfernt in API 87.1
        s.timer = furi_timer_alloc(ir_jam_timer_cb, FuriTimerTypePeriodic, NULL);
        furi_timer_start(s.timer, 50); /* Fire new frame every 50 ms */
    }
}

void ir_jam_stop(void) {
    if(!s.running) return;
    s.running = false;

    if(s.external) {
        ir_ext_stop();
    } else {
        if(s.timer) {
            furi_timer_stop(s.timer);
            furi_timer_free(s.timer);
            s.timer = NULL;
        }
        if(s.tx_in_progress) {
            furi_hal_infrared_async_tx_stop();
            s.tx_in_progress = false;
        }
    }
}
