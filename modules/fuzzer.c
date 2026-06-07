#include "fuzzer.h"
#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_rfid.h>
#include <furi_hal_nfc.h>
#include <string.h>

/* ------------------------------------------------------------------ */
/*  Shared state                                                        */
/* ------------------------------------------------------------------ */

typedef struct {
    bool       running;
    FuriTimer* timer;
    uint32_t   count;
    bool       is_nfc; /* false = RFID, true = NFC */
} FuzzerState;

static FuzzerState s = {0};

/* ------------------------------------------------------------------ */
/*  PRNG                                                                */
/* ------------------------------------------------------------------ */

static uint32_t xorshift(uint32_t* state) {
    uint32_t x = *state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *state = x;
    return x;
}

/* ------------------------------------------------------------------ */
/*  Timer callbacks                                                     */
/* ------------------------------------------------------------------ */

static void rfid_fuzzer_cb(void* ctx) {
    (void)ctx;
    if(!s.running) return;

    static uint32_t rng_state = 0x12345678;
    uint32_t id = xorshift(&rng_state);
    (void)id;

    /* Carrier kurz unterbrechen → simuliert neue Übertragung */
    furi_hal_rfid_tim_read_stop();
    furi_delay_us(500);
    furi_hal_rfid_tim_read_start(125000, 0.5f);

    s.count++;
}

static void nfc_fuzzer_cb(void* ctx) {
    (void)ctx;
    if(!s.running) return;

    /* NFC-Fuzzing: Feld kurz aus/ein schalten → stört 13.56 MHz Reader */
    furi_hal_nfc_abort();
    furi_delay_us(200);
    furi_hal_nfc_poller_field_on();

    s.count++;
}

/* ------------------------------------------------------------------ */
/*  Public API                                                          */
/* ------------------------------------------------------------------ */

void rfid_fuzzer_start(void) {
    if(s.running) rfid_fuzzer_stop();

    memset(&s, 0, sizeof(s));
    s.running = true;
    s.is_nfc  = false;

    furi_hal_rfid_tim_read_start(125000, 0.5f);

    s.timer = furi_timer_alloc(rfid_fuzzer_cb, FuriTimerTypePeriodic, NULL);
    furi_timer_start(s.timer, 50);
}

void rfid_fuzzer_stop(void) {
    if(!s.running) return;
    s.running = false;

    if(s.timer) {
        furi_timer_stop(s.timer);
        furi_timer_free(s.timer);
        s.timer = NULL;
    }

    if(!s.is_nfc) {
        furi_hal_rfid_tim_read_stop();
    }
}

void nfc_fuzzer_start(void) {
    if(s.running) nfc_fuzzer_stop();

    memset(&s, 0, sizeof(s));
    s.running = true;
    s.is_nfc  = true;

    furi_hal_nfc_acquire();
    furi_hal_nfc_poller_field_on();

    s.timer = furi_timer_alloc(nfc_fuzzer_cb, FuriTimerTypePeriodic, NULL);
    furi_timer_start(s.timer, 100);
}

void nfc_fuzzer_stop(void) {
    if(!s.running) return;
    s.running = false;

    if(s.timer) {
        furi_timer_stop(s.timer);
        furi_timer_free(s.timer);
        s.timer = NULL;
    }

    if(s.is_nfc) {
        furi_hal_nfc_abort();
        furi_hal_nfc_release();
    }
}

uint32_t fuzzer_iteration_count(void) {
    return s.count;
}
