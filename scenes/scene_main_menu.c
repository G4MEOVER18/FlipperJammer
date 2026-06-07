#include "../jammer_app.h"
#include "scenes.h"
#include "../resources/info_texts.h"

typedef enum {
    MainMenuSubGhz = 0,
    MainMenuNrf24,
    MainMenuBle,
    MainMenuWifi,
    MainMenuIr,
    MainMenuRfidNfc,
    MainMenuUsb,
    MainMenuDiag,
    MainMenuInfo,
} MainMenuIndex;

static void main_menu_callback(void* context, uint32_t index) {
    JammerApp* app = context;
    switch(index) {
    case MainMenuSubGhz:
        scene_manager_next_scene(app->scene_manager, SceneSubGhzConfig);
        break;
    case MainMenuNrf24:
        // Trotzdem zulassen — Config-Scene warnt, wenn !hw_nrf24
        scene_manager_next_scene(app->scene_manager, SceneNrf24Config);
        break;
    case MainMenuBle:
        scene_manager_next_scene(app->scene_manager, SceneBleConfig);
        break;
    case MainMenuWifi:
        scene_manager_next_scene(app->scene_manager, SceneWifiConfig);
        break;
    case MainMenuIr:
        scene_manager_next_scene(app->scene_manager, SceneIrConfig);
        break;
    case MainMenuRfidNfc:
        scene_manager_next_scene(app->scene_manager, SceneRfidMenu);
        break;
    case MainMenuUsb:
        scene_manager_next_scene(app->scene_manager, SceneUsbFuzzerConfig);
        break;
    case MainMenuDiag:
        scene_manager_next_scene(app->scene_manager, SceneNrf24Diag);
        break;
    case MainMenuInfo:
        app->info_text = INFO_MAIN;
        scene_manager_next_scene(app->scene_manager, SceneInfo);
        break;
    default:
        break;
    }
}

void jammer_scene_MainMenu_on_enter(void* context) {
    JammerApp* app = context;
    submenu_reset(app->submenu);

    // Header zeigt erkannte Multiboard-Module
    static char header_buf[40];
    snprintf(header_buf, sizeof(header_buf), "Jammer Suite  %s%s",
        app->hw_ext_cc1101 ? "[CC] " : "",
        app->hw_nrf24      ? "[NRF]" : "");
    submenu_set_header(app->submenu, header_buf);

    // SubGHz: Hinweis wenn ext CC1101 erkannt
    static char subghz_label[24];
    snprintf(subghz_label, sizeof(subghz_label), "SubGHz%s",
        app->hw_ext_cc1101 ? " [EXT]" : "");
    submenu_add_item(app->submenu, subghz_label,     MainMenuSubGhz,  main_menu_callback, app);

    // NRF24/BLE brauchen NRF24-Modul
    static char nrf24_label[28];
    snprintf(nrf24_label, sizeof(nrf24_label), "2.4GHz NRF24 %s",
        app->hw_nrf24 ? "[OK]" : "[!HW]");
    submenu_add_item(app->submenu, nrf24_label,      MainMenuNrf24,   main_menu_callback, app);

    static char ble_label[28];
    snprintf(ble_label, sizeof(ble_label), "BLE Jammer %s",
        app->hw_nrf24 ? "[OK]" : "[!HW]");
    submenu_add_item(app->submenu, ble_label,        MainMenuBle,     main_menu_callback, app);
    submenu_add_item(app->submenu, "WiFi Deauth (L2)",MainMenuWifi,    main_menu_callback, app);
    submenu_add_item(app->submenu, "IR Jammer",      MainMenuIr,      main_menu_callback, app);
    submenu_add_item(app->submenu, "RFID / NFC",     MainMenuRfidNfc, main_menu_callback, app);
    submenu_add_item(app->submenu, "USB Fuzzer",     MainMenuUsb,     main_menu_callback, app);
    submenu_add_item(app->submenu, "NRF24 Diagnose", MainMenuDiag,    main_menu_callback, app);
    submenu_add_item(app->submenu, "Info / Hilfe",   MainMenuInfo,    main_menu_callback, app);

    view_dispatcher_switch_to_view(app->view_dispatcher, ViewSubmenu);
}

bool jammer_scene_MainMenu_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void jammer_scene_MainMenu_on_exit(void* context) {
    JammerApp* app = context;
    submenu_reset(app->submenu);
}
