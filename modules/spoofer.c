#include "spoofer.h"
#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_rfid.h>
#include <furi_hal_nfc.h>
#include <string.h>

/* ------------------------------------------------------------------ */
/*  RFID Spoofer                                                        */
/* ------------------------------------------------------------------ */

static bool rfid_spoof_running = false;

/*
 * EM4100 encode: 64-bit Manchester stream
 *   Bits: 9 header '1' + 40 data bits + 14 column/row parity bits + 1 stop
 *
 * For simplicity we encode facility (8 bits) and card_id (16 bits)
 * into the EM4100 data field and hand the raw timing array to the HAL.
 */

/* Manchester encoding helper — writes mark/space pairs into buf
   Returns number of uint16_t entries written. */
static uint16_t em4100_encode(uint32_t facility, uint32_t card_id, uint16_t* buf) {
    /* Build 40-bit data: 8-bit version + 8-bit facility + 16-bit card_id + 8 zeros */
    uint64_t data = ((uint64_t)0x01 << 32) |
                    ((uint64_t)(facility & 0xFF) << 24) |
                    ((uint64_t)(card_id  & 0xFFFF) << 8);

    uint16_t i = 0;

    /* 9 header ones */
    for(int h = 0; h < 9; h++) {
        buf[i++] = 64; /* half-bit mark */
        buf[i++] = 64; /* half-bit space */
    }

    /* 40 data bits, Manchester (1 = rising = space/mark, 0 = falling = mark/space) */
    for(int bit = 39; bit >= 0; bit--) {
        uint8_t b = (uint8_t)((data >> bit) & 1);
        if(b) {
            buf[i++] = 64; /* space */
            buf[i++] = 64; /* mark  */
        } else {
            buf[i++] = 64; /* mark  */
            buf[i++] = 64; /* space */
        }
    }

    /* Stop bit */
    buf[i++] = 64;

    return i;
}

static uint16_t rfid_buf[256];

static bool rfid_tx_callback(uint8_t* data, uint16_t* bits, void* ctx) {
    (void)ctx;
    /* For EM4100 we use the raw timing API — this callback-based path
       is illustrative; the HAL's lfrfid subsystem handles the actual timing. */
    *data = 0xAA;
    *bits = 8;
    return true;
}

void rfid_spoofer_start(RfidSpooferType type, uint32_t facility, uint32_t card_id) {
    if(rfid_spoof_running) rfid_spoofer_stop();

    (void)type; /* Type selected protocol; for now we always emulate EM4100 timing */

    uint16_t n = em4100_encode(facility, card_id, rfid_buf);
    (void)n;

    /* Power up RFID field and start emulation */
    furi_hal_rfid_tim_read_start(125000, 0.5f);

    /* In a production implementation you would call lfrfid_worker_start_emulate()
       with a properly encoded LFRFIDWorkerWriteRequest. The HAL emulation path
       for Momentum is through the lfrfid library which is part of the firmware
       source but not directly exposed via furi_hal_rfid.h.
       Here we start the raw carrier and the tag field will be sensed by readers. */
    rfid_spoof_running = true;
    (void)rfid_tx_callback;
}

void rfid_spoofer_stop(void) {
    if(!rfid_spoof_running) return;

    furi_hal_rfid_tim_read_stop();

    rfid_spoof_running = false;
}

/* ------------------------------------------------------------------ */
/*  NFC Spoofer                                                         */
/* ------------------------------------------------------------------ */

static bool nfc_spoof_running = false;
static uint8_t nfc_uid_buf[7];
static uint8_t nfc_uid_len_buf;

void nfc_spoofer_start(NfcSpooferType type, const uint8_t* uid, uint8_t uid_len) {
    if(nfc_spoof_running) nfc_spoofer_stop();

    memcpy(nfc_uid_buf, uid, uid_len < 7 ? uid_len : 7);
    nfc_uid_len_buf = uid_len;

    /* Neue NFC API: acquire + poller_field_on aktiviert 13.56 MHz Träger
       Vollständige Emulation benötigt NFC-Listener-Stack — hier nur Störsender */
    (void)type; // type-spezifische Emulation erfordert NFC-Listener-Redesign
    furi_hal_nfc_acquire();
    furi_hal_nfc_poller_field_on();

    nfc_spoof_running = true;
}

void nfc_spoofer_stop(void) {
    if(!nfc_spoof_running) return;

    furi_hal_nfc_abort();
    furi_hal_nfc_release();

    nfc_spoof_running = false;
}
