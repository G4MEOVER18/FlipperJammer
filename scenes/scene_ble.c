#include "../jammer_app.h"
#include "scenes.h"
#include "../modules/ble_jam.h"
#include <stdio.h>

/* ------------------------------------------------------------------ */
/*  BLE Config Screen — zeigt NRF24-Modulstatus, Pinbelegung, Dauer   */
/* ------------------------------------------------------------------ */

typedef enum {
    BleCfgInfo = 0,
    BleCfgDuration,
    BleCfgStart,
} BleCfgIndex;

static void ble_config_enter_cb(void* context, uint32_t index) {
    JammerApp* app = context;
    if(index == BleCfgDuration) {
        app->active_module = SceneBleRun;
        scene_manager_next_scene(app->scene_manager, SceneDuration);
    } else if(index == BleCfgStart) {
        scene_manager_next_scene(app->scene_manager, SceneBleRun);
    }
}

void jammer_scene_BleConfig_on_enter(void* context) {
    JammerApp* app = context;

    variable_item_list_reset(app->var_list);
    variable_item_list_set_header(app->var_list, "BLE Konfiguration");

    // Modul-Status (read-only Info-Zeile)
    const char* modul_str = app->hw_nrf24 ? "NRF24 OK" : "NRF24 nicht gefunden";
    VariableItem* item = variable_item_list_add(
        app->var_list, "Modul", 1, NULL, app);
    variable_item_set_current_value_text(item, modul_str);

    variable_item_list_add(app->var_list, "Dauer waehlen...", 1, NULL, app);
    variable_item_list_add(app->var_list, ">> START <<",      1, NULL, app);

    variable_item_list_set_enter_callback(app->var_list, ble_config_enter_cb, app);
    view_dispatcher_switch_to_view(app->view_dispatcher, ViewVarList);
}

bool jammer_scene_BleConfig_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void jammer_scene_BleConfig_on_exit(void* context) {
    JammerApp* app = context;
    variable_item_list_reset(app->var_list);
}

/* BLE Run screen */

static void ble_run_update_widget(JammerApp* app) {
    widget_reset(app->widget);

    char line_time[32];
    if(app->total_ms == 0) {
        snprintf(line_time, sizeof(line_time), "Zeit: Unbegrenzt");
    } else {
        uint32_t remaining = (app->total_ms - app->elapsed_ms) / 1000;
        snprintf(line_time, sizeof(line_time), "Restzeit: %lu s", (unsigned long)remaining);
    }

    const char* modul_str = app->hw_nrf24 ? "NRF24 Multiboard" : "NRF24 !nicht erkannt";
    widget_add_string_element(app->widget, 64,  2, AlignCenter, AlignTop, FontPrimary,   "BLE JAMMER AKTIV");
    widget_add_string_element(app->widget,  2, 18, AlignLeft,   AlignTop, FontSecondary, modul_str);
    widget_add_string_element(app->widget,  2, 30, AlignLeft,   AlignTop, FontSecondary, "Kanaele: 37/38/39  1ms/Hop");
    widget_add_string_element(app->widget,  2, 42, AlignLeft,   AlignTop, FontSecondary, line_time);
    widget_add_string_element(app->widget, 64, 55, AlignCenter, AlignTop, FontSecondary, "[BACK] Stopp");
}

void jammer_scene_BleRun_on_enter(void* context) {
    JammerApp* app = context;

    app->elapsed_ms = 0;
    app->total_ms   = app->duration_ms;
    app->running    = true;

    ble_jam_start();
    jammer_notify_start(app);

    ble_run_update_widget(app);
    view_dispatcher_switch_to_view(app->view_dispatcher, ViewWidget);

    furi_timer_start(app->run_timer, 500);
}

bool jammer_scene_BleRun_on_event(void* context, SceneManagerEvent event) {
    JammerApp* app     = context;
    bool       handled = false;

    if(event.type == SceneManagerEventTypeCustom && event.event == 0xDEAD) {
        ble_jam_stop();
        jammer_notify_stop(app);
        scene_manager_previous_scene(app->scene_manager);
        handled = true;
    } else if(event.type == SceneManagerEventTypeTick) {
        ble_run_update_widget(app);
        handled = true;
    }
    return handled;
}

void jammer_scene_BleRun_on_exit(void* context) {
    JammerApp* app = context;
    furi_timer_stop(app->run_timer);
    app->running = false;
    ble_jam_stop();
    widget_reset(app->widget);
}
