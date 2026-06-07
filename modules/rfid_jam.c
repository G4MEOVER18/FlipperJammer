#include "rfid_jam.h"
#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_rfid.h>

static bool rfid_jam_running = false;

void rfid_jam_start_carrier(void) {
    if(rfid_jam_running) return;

    /* Start 125 kHz carrier with 50 % duty cycle */
    furi_hal_rfid_tim_read_start(125000, 0.5f);

    rfid_jam_running = true;
}

void rfid_jam_stop(void) {
    if(!rfid_jam_running) return;

    furi_hal_rfid_tim_read_stop();

    rfid_jam_running = false;
}
