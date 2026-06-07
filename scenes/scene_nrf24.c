#include "../jammer_app.h"
#include "scenes.h"
#include "../modules/nrf24_jam.h"
#include <stdio.h>

static const char* nrf24_mode_labels[] = {
    "ALL Kanaele",
    "Bluetooth",
    "Zigbee",
    "WiFi-Kanal 1",
    "WiFi-Kanal 6",
    "WiFi-Kanal 11",
    "Logitech",
};

/* ------------------------------------------------------------------ */
/*  Config                                                              */
/* ------------------------------------------------------------------ */

static void nrf24_mode_change(VariableItem* item) {
    JammerApp* app   = variable_item_get_context(item);
    uint8_t    index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, nrf24_mode_labels[index]);
    app->nrf24_mode = (Nrf24Mode)index;
}

typedef enum {
    Nrf24CfgMode = 0,
    Nrf24CfgDuration,
    Nrf24CfgStart,
} Nrf24CfgIndex;

static void nrf24_config_enter(void* context, uint32_t index) {
    JammerApp* app = context;
    if(index == Nrf24CfgDuration) {
        app->active_module = SceneNrf24Run;
        scene_manager_next_scene(app->scene_manager, SceneDuration);
    } else if(index == Nrf24CfgStart) {
        scene_manager_next_scene(app->scene_manager, SceneNrf24Run);
    }
}

void jammer_scene_Nrf24Config_on_enter(void* context) {
    JammerApp*    app = context;
    VariableItem* item;

    variable_item_list_reset(app->var_list);
    variable_item_list_set_header(app->var_list, "NRF24 Konfiguration");

    item = variable_item_list_add(
        app->var_list, "Modus", 7, nrf24_mode_change, app);
    variable_item_set_current_value_index(item, (uint8_t)app->nrf24_mode);
    variable_item_set_current_value_text(item, nrf24_mode_labels[(uint8_t)app->nrf24_mode]);

    variable_item_list_add(app->var_list, "Dauer waehlen...", 1, NULL, app);
    variable_item_list_add(app->var_list, ">> START <<",      1, NULL, app);

    variable_item_list_set_enter_callback(app->var_list, nrf24_config_enter, app);
    view_dispatcher_switch_to_view(app->view_dispatcher, ViewVarList);
}

bool jammer_scene_Nrf24Config_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void jammer_scene_Nrf24Config_on_exit(void* context) {
    JammerApp* app = context;
    variable_item_list_reset(app->var_list);
}

/* ------------------------------------------------------------------ */
/*  Run Screen                                                          */
/* ------------------------------------------------------------------ */

static void nrf24_run_update_widget(JammerApp* app) {
    widget_reset(app->widget);

    char line_ch[32];
    snprintf(line_ch, sizeof(line_ch), "Kanal: %u", (unsigned)nrf24_jam_current_channel());

    char line_time[32];
    if(app->total_ms == 0) {
        snprintf(line_time, sizeof(line_time), "Zeit: Unbegrenzt");
    } else {
        uint32_t remaining = (app->total_ms - app->elapsed_ms) / 1000;
        snprintf(line_time, sizeof(line_time), "Restzeit: %lu s", (unsigned long)remaining);
    }

    widget_add_string_element(app->widget, 64, 2,  AlignCenter, AlignTop, FontPrimary,   "NRF24 AKTIV");
    widget_add_string_element(app->widget, 2,  18, AlignLeft,   AlignTop, FontSecondary, nrf24_mode_labels[(uint8_t)app->nrf24_mode]);
    widget_add_string_element(app->widget, 2,  30, AlignLeft,   AlignTop, FontSecondary, line_ch);
    widget_add_string_element(app->widget, 2,  42, AlignLeft,   AlignTop, FontSecondary, line_time);
    widget_add_string_element(app->widget, 64, 55, AlignCenter, AlignTop, FontSecondary, "[BACK] Stopp");
}

void jammer_scene_Nrf24Run_on_enter(void* context) {
    JammerApp* app = context;

    app->elapsed_ms = 0;
    app->total_ms   = app->duration_ms;
    app->running    = true;

    nrf24_jam_start((Nrf24JamMode)app->nrf24_mode);
    jammer_notify_start(app);

    nrf24_run_update_widget(app);
    view_dispatcher_switch_to_view(app->view_dispatcher, ViewWidget);

    furi_timer_start(app->run_timer, 500);
}

bool jammer_scene_Nrf24Run_on_event(void* context, SceneManagerEvent event) {
    JammerApp* app     = context;
    bool       handled = false;

    if(event.type == SceneManagerEventTypeCustom && event.event == 0xDEAD) {
        nrf24_jam_stop();
        jammer_notify_stop(app);
        scene_manager_previous_scene(app->scene_manager);
        handled = true;
    } else if(event.type == SceneManagerEventTypeTick) {
        nrf24_run_update_widget(app);
        handled = true;
    }
    return handled;
}

void jammer_scene_Nrf24Run_on_exit(void* context) {
    JammerApp* app = context;
    furi_timer_stop(app->run_timer);
    app->running = false;
    nrf24_jam_stop();
    widget_reset(app->widget);
}
