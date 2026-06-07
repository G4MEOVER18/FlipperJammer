#include "nfc_jam.h"
#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_nfc.h>

static bool nfc_jam_running = false;

void nfc_jam_start(void) {
    if(nfc_jam_running) return;

    /* Neue NFC API: acquire + poller_field_on für 13.56 MHz Träger */
    furi_hal_nfc_acquire();
    furi_hal_nfc_poller_field_on();

    nfc_jam_running = true;
}

void nfc_jam_stop(void) {
    if(!nfc_jam_running) return;

    furi_hal_nfc_abort();
    furi_hal_nfc_release();

    nfc_jam_running = false;
}
