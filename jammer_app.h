#pragma once
#define JAMMER_APP_H_INCLUDED

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/submenu.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/widget.h>
#include <notification/notification_messages.h>

/* ------------------------------------------------------------------ */
/*  Enumerations                                                        */
/* ------------------------------------------------------------------ */

typedef enum {
    SceneMainMenu,
    SceneSubGhzConfig,
    SceneSubGhzRun,
    SceneNrf24Config,
    SceneNrf24Run,
    SceneBleRun,
    SceneWifiConfig,
    SceneWifiRun,
    SceneIrConfig,
    SceneIrRun,
    SceneRfidMenu,
    SceneRfidJamRun,
    SceneNfcJamRun,
    SceneRfidSpooferConfig,
    SceneNfcSpooferConfig,
    SceneRfidFuzzerRun,
    SceneNfcFuzzerRun,
    SceneUsbFuzzerConfig,
    SceneUsbFuzzerRun,
    SceneDuration,
    SceneInfo,
    SceneCount,
} JammerScene;

typedef enum {
    ViewSubmenu,
    ViewVarList,
    ViewWidget,
    ViewCount,
} JammerView;

/* SubGHz */
typedef enum {
    SubGhzModeCarrier = 0,
    SubGhzModeNoise,
    SubGhzModeSweep,
} SubGhzMode;

typedef enum {
    SubGhzPreset433 = 0,
    SubGhzPreset868,
    SubGhzPreset315,
    SubGhzPreset915,
} SubGhzPreset;

/* IR */
typedef enum {
    IrModeNoiseBurst = 0,
    IrModeTvCodes,
    IrModeRandomRaw,
    IrModeNecFlood,
    IrModeSamsung,
    IrModeSony,
} IrMode;

/* NRF24 */
typedef enum {
    Nrf24ModeAll = 0,
    Nrf24ModeBt,
    Nrf24ModeZigbee,
    Nrf24ModeWifi1,
    Nrf24ModeWifi6,
    Nrf24ModeWifi11,
    Nrf24ModeLogitech,
} Nrf24Mode;

/* Duration */
typedef enum {
    Duration10s = 0,
    Duration30s,
    Duration1m,
    Duration5m,
    Duration10m,
    DurationUnlimited,
} DurationOption;

/* USB Fuzz */
typedef enum {
    UsbFuzzHidFlood = 0,
    UsbFuzzMalformed,
    UsbFuzzDescriptor,
    UsbFuzzReconnect,
} UsbFuzzMode;

/* ------------------------------------------------------------------ */
/*  App State                                                           */
/* ------------------------------------------------------------------ */

typedef struct {
    /* GUI components */
    Gui*              gui;
    ViewDispatcher*   view_dispatcher;
    SceneManager*     scene_manager;
    Submenu*          submenu;
    VariableItemList* var_list;
    Widget*           widget;

    /* SubGHz config */
    SubGhzMode    subghz_mode;
    SubGhzPreset  subghz_preset;
    bool          subghz_external; /* true = REK5Lab */

    /* IR config */
    IrMode ir_mode;
    bool   ir_external; /* true = Rabbit Labs IR Blaster */

    /* NRF24 config */
    Nrf24Mode nrf24_mode;

    /* WiFi config */
    uint8_t wifi_channel; /* 0 = ALL, 1-13 = specific */
    bool    wifi_broadcast;
    char    wifi_bssid[18]; /* "XX:XX:XX:XX:XX:XX\0" */

    /* USB Fuzzer */
    UsbFuzzMode usb_mode;

    /* Duration */
    DurationOption duration;
    uint32_t       duration_ms;  /* 0 = unlimited */

    /* Runtime */
    bool       running;
    FuriTimer* run_timer;
    uint32_t   elapsed_ms;
    uint32_t   total_ms;

    /* Scene routing helper */
    JammerScene active_module; /* which run-scene to go to after duration */

    /* Info screen */
    const char* info_text;
} JammerApp;

/* ------------------------------------------------------------------ */
/*  Public API                                                          */
/* ------------------------------------------------------------------ */

JammerApp* jammer_app_alloc(void);
void       jammer_app_free(JammerApp* app);

/* Timer callback — linked to run_timer */
void jammer_timer_callback(void* context);

/* Converts DurationOption → milliseconds (0 = unlimited) */
uint32_t jammer_duration_to_ms(DurationOption opt);

/* Notification helpers */
void jammer_notify_start(JammerApp* app);
void jammer_notify_stop(JammerApp* app);
