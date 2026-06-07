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
    MainMenuInfo,
} MainMenuIndex;

static void main_menu_callback(void* context, uint32_t index) {
    JammerApp* app = context;
    switch(index) {
    case MainMenuSubGhz:
        scene_manager_next_scene(app->scene_manager, SceneSubGhzConfig);
        break;
    case MainMenuNrf24:
        scene_manager_next_scene(app->scene_manager, SceneNrf24Config);
        break;
    case MainMenuBle:
        app->active_module = SceneBleRun;
        scene_manager_next_scene(app->scene_manager, SceneDuration);
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
    submenu_set_header(app->submenu, "Jammer Suite v1.0");

    submenu_add_item(app->submenu, "SubGHz Jammer",  MainMenuSubGhz,  main_menu_callback, app);
    submenu_add_item(app->submenu, "2.4GHz NRF24",   MainMenuNrf24,   main_menu_callback, app);
    submenu_add_item(app->submenu, "BLE Jammer",     MainMenuBle,     main_menu_callback, app);
    submenu_add_item(app->submenu, "WiFi Jammer",    MainMenuWifi,    main_menu_callback, app);
    submenu_add_item(app->submenu, "IR Jammer",      MainMenuIr,      main_menu_callback, app);
    submenu_add_item(app->submenu, "RFID / NFC",     MainMenuRfidNfc, main_menu_callback, app);
    submenu_add_item(app->submenu, "USB Fuzzer",     MainMenuUsb,     main_menu_callback, app);
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
