#pragma once
#include "../jammer_app.h"

#define JAMMER_STORAGE_DIR  "/ext/apps_data/jammer_suite"
#define JAMMER_STORAGE_FILE "/ext/apps_data/jammer_suite/config.bin"
#define JAMMER_STORAGE_MAGIC  0x4A414D31  // "JAM1"
#define JAMMER_STORAGE_VERSION 1

/* Persistente Settings. Versioned struct, MAGIC im Header — alte/neue
   Versionen werden ignoriert (Reset auf Defaults). */
typedef struct {
    uint32_t magic;
    uint32_t version;
    uint8_t  subghz_mode;
    uint8_t  subghz_preset;
    uint8_t  subghz_external;
    uint8_t  ir_mode;
    uint8_t  ir_external;
    uint8_t  nrf24_mode;
    uint8_t  nrf24_power;     // 0=-18dBm, 1=-12, 2=-6, 3=0dBm
    uint8_t  wifi_channel;
    uint8_t  wifi_broadcast;
    char     wifi_bssid[18];
    uint8_t  usb_mode;
    uint8_t  duration;
    uint8_t  _reserved[16];
} JammerSettings;

/* Settings aus Datei laden. Wenn Datei fehlt/korrupt: Defaults bleiben. */
void jammer_settings_load(JammerApp* app);

/* Settings in Datei speichern. */
void jammer_settings_save(JammerApp* app);
