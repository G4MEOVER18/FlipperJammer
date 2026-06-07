#include "../jammer_app.h"
#include "scenes.h"

typedef enum {
    RfidMenuRfidJam = 0,
    RfidMenuNfcJam,
    RfidMenuRfidSpoofer,
    RfidMenuNfcSpoofer,
    RfidMenuRfidFuzzer,
    RfidMenuNfcFuzzer,
} RfidMenuIndex;

static void rfid_menu_callback(void* context, uint32_t index) {
    JammerApp* app = context;
    switch(index) {
    case RfidMenuRfidJam:
        app->active_module = SceneRfidJamRun;
        scene_manager_next_scene(app->scene_manager, SceneDuration);
        break;
    case RfidMenuNfcJam:
        app->active_module = SceneNfcJamRun;
        scene_manager_next_scene(app->scene_manager, SceneDuration);
        break;
    case RfidMenuRfidSpoofer:
        scene_manager_next_scene(app->scene_manager, SceneRfidSpooferConfig);
        break;
    case RfidMenuNfcSpoofer:
        scene_manager_next_scene(app->scene_manager, SceneNfcSpooferConfig);
        break;
    case RfidMenuRfidFuzzer:
        app->active_module = SceneRfidFuzzerRun;
        scene_manager_next_scene(app->scene_manager, SceneDuration);
        break;
    case RfidMenuNfcFuzzer:
        app->active_module = SceneNfcFuzzerRun;
        scene_manager_next_scene(app->scene_manager, SceneDuration);
        break;
    default:
        break;
    }
}

void jammer_scene_RfidMenu_on_enter(void* context) {
    JammerApp* app = context;
    submenu_reset(app->submenu);
    submenu_set_header(app->submenu, "RFID / NFC");

    submenu_add_item(app->submenu, "RFID Jammer",    RfidMenuRfidJam,      rfid_menu_callback, app);
    submenu_add_item(app->submenu, "NFC Jammer",     RfidMenuNfcJam,       rfid_menu_callback, app);
    submenu_add_item(app->submenu, "RFID Spoofer",   RfidMenuRfidSpoofer,  rfid_menu_callback, app);
    submenu_add_item(app->submenu, "NFC Spoofer",    RfidMenuNfcSpoofer,   rfid_menu_callback, app);
    submenu_add_item(app->submenu, "RFID Fuzzer",    RfidMenuRfidFuzzer,   rfid_menu_callback, app);
    submenu_add_item(app->submenu, "NFC Fuzzer",     RfidMenuNfcFuzzer,    rfid_menu_callback, app);

    view_dispatcher_switch_to_view(app->view_dispatcher, ViewSubmenu);
}

bool jammer_scene_RfidMenu_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void jammer_scene_RfidMenu_on_exit(void* context) {
    JammerApp* app = context;
    submenu_reset(app->submenu);
}
