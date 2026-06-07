#include "usb_fuzz.h"
#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_usb.h>
#include <furi_hal_usb_hid.h>

/* ------------------------------------------------------------------ */
/*  State                                                               */
/* ------------------------------------------------------------------ */

typedef struct {
    bool        running;
    UsbFuzzMode mode;
    FuriTimer*  timer;
    uint32_t    reconnect_phase; /* 0 = USB off, 1 = USB on */
} UsbFuzzState;

static UsbFuzzState s = {0};

/* ------------------------------------------------------------------ */
/*  HID report builders                                                 */
/* ------------------------------------------------------------------ */

static uint32_t usb_rng = 0xABCDEF01;

static uint32_t xorshift32(void) {
    usb_rng ^= usb_rng << 13;
    usb_rng ^= usb_rng >> 17;
    usb_rng ^= usb_rng << 5;
    return usb_rng;
}

static void send_hid_keyboard_report(uint8_t modifier, uint8_t keycode) {
    // Neue API: furi_hal_hid_kb_press(uint16_t button) — 1 Argument
    // Modifier-Bits liegen in Bits 8-15, Keycode in Bits 0-7
    uint16_t button = ((uint16_t)modifier << 8) | (uint16_t)keycode;
    furi_hal_hid_kb_press(button);
    furi_delay_ms(5);
    furi_hal_hid_kb_release(button);
}

/* ------------------------------------------------------------------ */
/*  Timer callbacks                                                     */
/* ------------------------------------------------------------------ */

static void hid_flood_cb(void* ctx) {
    (void)ctx;
    if(!s.running) return;

    uint32_t rng = xorshift32();
    uint8_t modifier = (uint8_t)(rng >> 8) & 0x0F; /* low 4 bits = valid modifier */
    uint8_t keycode  = (uint8_t)(rng & 0x7F);

    send_hid_keyboard_report(modifier, keycode);
}

static void malformed_hid_cb(void* ctx) {
    (void)ctx;
    if(!s.running) return;

    /* Send HID reports with random lengths and IDs to cause driver confusion */
    uint32_t rng = xorshift32();
    /* Alternate between valid and invalid keycodes */
    uint8_t keycode  = (uint8_t)(rng | 0x80);
    uint8_t modifier = (uint8_t)((rng >> 8) & 0xFF);
    uint16_t button  = ((uint16_t)modifier << 8) | (uint16_t)keycode;
    furi_hal_hid_kb_press(button);
    furi_delay_ms(1);
    furi_hal_hid_kb_release_all();
}

static void reconnect_cb(void* ctx) {
    (void)ctx;
    if(!s.running) return;

    if(s.reconnect_phase == 0) {
        /* Disconnect */
        furi_hal_usb_set_config(NULL, NULL);
        s.reconnect_phase = 1;
    } else {
        /* Reconnect as HID */
        furi_hal_usb_set_config(&usb_hid, NULL);
        s.reconnect_phase = 0;
    }
}

/* ------------------------------------------------------------------ */
/*  Public API                                                          */
/* ------------------------------------------------------------------ */

void usb_fuzz_start(UsbFuzzMode mode) {
    if(s.running) usb_fuzz_stop();

    s.mode            = mode;
    s.running         = true;
    s.reconnect_phase = 0;
    usb_rng           = (uint32_t)furi_get_tick() ^ 0xCAFEBABE;

    switch(mode) {
    case UsbFuzzHidFlood:
        furi_hal_usb_set_config(&usb_hid, NULL);
        furi_delay_ms(50);
        s.timer = furi_timer_alloc(hid_flood_cb, FuriTimerTypePeriodic, NULL);
        furi_timer_start(s.timer, 10); /* 100 keypresses/s */
        break;

    case UsbFuzzMalformed:
        furi_hal_usb_set_config(&usb_hid, NULL);
        furi_delay_ms(50);
        s.timer = furi_timer_alloc(malformed_hid_cb, FuriTimerTypePeriodic, NULL);
        furi_timer_start(s.timer, 5);
        break;

    case UsbFuzzDescriptor:
        /* Descriptor fuzzing: cycle through USB HID config rapidly.
           Each set_config re-enumerates the device, which causes the host
           to re-read the descriptor. We do this at ~1 Hz. */
        s.timer = furi_timer_alloc(reconnect_cb, FuriTimerTypePeriodic, NULL);
        furi_timer_start(s.timer, 500);
        break;

    case UsbFuzzReconnect:
        /* Fast reconnect loop */
        s.timer = furi_timer_alloc(reconnect_cb, FuriTimerTypePeriodic, NULL);
        furi_timer_start(s.timer, 10);
        break;
    }
}

void usb_fuzz_stop(void) {
    if(!s.running) return;
    s.running = false;

    if(s.timer) {
        furi_timer_stop(s.timer);
        furi_timer_free(s.timer);
        s.timer = NULL;
    }

    /* Release all keys if HID mode was active */
    if(s.mode == UsbFuzzHidFlood || s.mode == UsbFuzzMalformed) {
        furi_hal_hid_kb_release_all();
    }

    /* Restore default USB serial config */
    furi_hal_usb_set_config(&usb_cdc_single, NULL);
}
