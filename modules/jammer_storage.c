#include "jammer_storage.h"
#include <storage/storage.h>
#include <string.h>

void jammer_settings_load(JammerApp* app) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    File* f = storage_file_alloc(storage);
    JammerSettings cfg = {0};

    if(storage_file_open(f, JAMMER_STORAGE_FILE, FSAM_READ, FSOM_OPEN_EXISTING)) {
        size_t n = storage_file_read(f, &cfg, sizeof(cfg));
        if(n == sizeof(cfg) &&
           cfg.magic == JAMMER_STORAGE_MAGIC &&
           cfg.version == JAMMER_STORAGE_VERSION) {
            app->subghz_mode     = (SubGhzMode)cfg.subghz_mode;
            app->subghz_preset   = (SubGhzPreset)cfg.subghz_preset;
            app->subghz_external = cfg.subghz_external;
            app->ir_mode         = (IrMode)cfg.ir_mode;
            app->ir_external     = cfg.ir_external;
            app->nrf24_mode      = (Nrf24Mode)cfg.nrf24_mode;
            app->nrf24_power     = cfg.nrf24_power;
            app->wifi_channel    = cfg.wifi_channel;
            app->wifi_broadcast  = cfg.wifi_broadcast;
            memcpy(app->wifi_bssid, cfg.wifi_bssid, sizeof(app->wifi_bssid));
            app->wifi_bssid[sizeof(app->wifi_bssid)-1] = 0;
            app->usb_mode        = (UsbFuzzMode)cfg.usb_mode;
            app->duration        = (DurationOption)cfg.duration;
            app->duration_ms     = jammer_duration_to_ms(app->duration);
        }
        storage_file_close(f);
    }
    storage_file_free(f);
    furi_record_close(RECORD_STORAGE);
}

void jammer_settings_save(JammerApp* app) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    storage_simply_mkdir(storage, JAMMER_STORAGE_DIR);

    File* f = storage_file_alloc(storage);
    JammerSettings cfg = {0};
    cfg.magic           = JAMMER_STORAGE_MAGIC;
    cfg.version         = JAMMER_STORAGE_VERSION;
    cfg.subghz_mode     = (uint8_t)app->subghz_mode;
    cfg.subghz_preset   = (uint8_t)app->subghz_preset;
    cfg.subghz_external = app->subghz_external ? 1 : 0;
    cfg.ir_mode         = (uint8_t)app->ir_mode;
    cfg.ir_external     = app->ir_external ? 1 : 0;
    cfg.nrf24_mode      = (uint8_t)app->nrf24_mode;
    cfg.nrf24_power     = app->nrf24_power;
    cfg.wifi_channel    = app->wifi_channel;
    cfg.wifi_broadcast  = app->wifi_broadcast ? 1 : 0;
    memcpy(cfg.wifi_bssid, app->wifi_bssid, sizeof(cfg.wifi_bssid));
    cfg.usb_mode        = (uint8_t)app->usb_mode;
    cfg.duration        = (uint8_t)app->duration;

    if(storage_file_open(f, JAMMER_STORAGE_FILE, FSAM_WRITE, FSOM_CREATE_ALWAYS)) {
        storage_file_write(f, &cfg, sizeof(cfg));
        storage_file_close(f);
    }
    storage_file_free(f);
    furi_record_close(RECORD_STORAGE);
}
