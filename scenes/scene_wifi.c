#include "../jammer_app.h"
#include "scenes.h"
#include "../modules/wifi_jam.h"
#include <stdio.h>

static const char* wifi_ch_labels[] = {
    "ALL", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13",
};
static const char* wifi_mode_labels[] = {"Broadcast", "Targeted"};

/* ------------------------------------------------------------------ */
/*  Config                                                              */
/* ------------------------------------------------------------------ */

static void wifi_ch_change(VariableItem* item) {
    JammerApp* app   = variable_item_get_context(item);
    uint8_t    index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, wifi_ch_labels[index]);
    app->wifi_channel = index; /* 0 = ALL, 1-13 = specific */
}

static void wifi_mode_change(VariableItem* item) {
    JammerApp* app   = variable_item_get_context(item);
    uint8_t    index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, wifi_mode_labels[index]);
    app->wifi_broadcast = (index == 0);
}

typedef enum {
    WifiCfgChannel = 0,
    WifiCfgMode,
    WifiCfgDuration,
    WifiCfgStart,
} WifiCfgIndex;

static void wifi_config_enter(void* context, uint32_t index) {
    JammerApp* app = context;
    if(index == WifiCfgDuration) {
        app->active_module = SceneWifiRun;
        scene_manager_next_scene(app->scene_manager, SceneDuration);
    } else if(index == WifiCfgStart) {
        scene_manager_next_scene(app->scene_manager, SceneWifiRun);
    }
}

void jammer_scene_WifiConfig_on_enter(void* context) {
    JammerApp*    app = context;
    VariableItem* item;

    variable_item_list_reset(app->var_list);
    variable_item_list_set_header(app->var_list, "WiFi Konfiguration");

    item = variable_item_list_add(app->var_list, "Kanal", 14, wifi_ch_change, app);
    variable_item_set_current_value_index(item, app->wifi_channel);
    variable_item_set_current_value_text(item, wifi_ch_labels[app->wifi_channel]);

    item = variable_item_list_add(app->var_list, "Modus", 2, wifi_mode_change, app);
    variable_item_set_current_value_index(item, app->wifi_broadcast ? 0 : 1);
    variable_item_set_current_value_text(item, wifi_mode_labels[app->wifi_broadcast ? 0 : 1]);

    variable_item_list_add(app->var_list, "Dauer waehlen...", 1, NULL, app);
    variable_item_list_add(app->var_list, ">> START <<",      1, NULL, app);

    variable_item_list_set_enter_callback(app->var_list, wifi_config_enter, app);
    view_dispatcher_switch_to_view(app->view_dispatcher, ViewVarList);
}

bool jammer_scene_WifiConfig_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void jammer_scene_WifiConfig_on_exit(void* context) {
    JammerApp* app = context;
    variable_item_list_reset(app->var_list);
}

/* ------------------------------------------------------------------ */
/*  Run Screen                                                          */
/* ------------------------------------------------------------------ */

static void wifi_run_update_widget(JammerApp* app) {
    widget_reset(app->widget);

    char line1[40];
    snprintf(line1, sizeof(line1), "Kanal: %s  Modus: %s",
        wifi_ch_labels[app->wifi_channel],
        wifi_mode_labels[app->wifi_broadcast ? 0 : 1]);

    char line_time[32];
    if(app->total_ms == 0) {
        snprintf(line_time, sizeof(line_time), "Zeit: Unbegrenzt");
    } else {
        uint32_t remaining = (app->total_ms - app->elapsed_ms) / 1000;
        snprintf(line_time, sizeof(line_time), "Restzeit: %lu s", (unsigned long)remaining);
    }

    widget_add_string_element(app->widget, 64,  2, AlignCenter, AlignTop, FontPrimary,   "WiFi JAMMER AKTIV");
    widget_add_string_element(app->widget,  2, 18, AlignLeft,   AlignTop, FontSecondary, line1);
    widget_add_string_element(app->widget,  2, 30, AlignLeft,   AlignTop, FontSecondary, "ESP32 Deauth via UART");
    widget_add_string_element(app->widget,  2, 42, AlignLeft,   AlignTop, FontSecondary, line_time);
    widget_add_string_element(app->widget, 64, 55, AlignCenter, AlignTop, FontSecondary, "[BACK] Stopp");
}

void jammer_scene_WifiRun_on_enter(void* context) {
    JammerApp* app = context;

    app->elapsed_ms = 0;
    app->total_ms   = app->duration_ms;
    app->running    = true;

    wifi_jam_init();
    wifi_jam_start(app->wifi_channel, app->wifi_broadcast, app->wifi_bssid);
    jammer_notify_start(app);

    wifi_run_update_widget(app);
    view_dispatcher_switch_to_view(app->view_dispatcher, ViewWidget);

    furi_timer_start(app->run_timer, 500);
}

bool jammer_scene_WifiRun_on_event(void* context, SceneManagerEvent event) {
    JammerApp* app     = context;
    bool       handled = false;

    if(event.type == SceneManagerEventTypeCustom && event.event == 0xDEAD) {
        wifi_jam_stop();
        jammer_notify_stop(app);
        scene_manager_previous_scene(app->scene_manager);
        handled = true;
    } else if(event.type == SceneManagerEventTypeTick) {
        wifi_run_update_widget(app);
        handled = true;
    }
    return handled;
}

void jammer_scene_WifiRun_on_exit(void* context) {
    JammerApp* app = context;
    furi_timer_stop(app->run_timer);
    app->running = false;
    wifi_jam_stop();
    widget_reset(app->widget);
}
