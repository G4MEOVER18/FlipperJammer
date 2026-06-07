#include "../jammer_app.h"
#include "scenes.h"
#include "../modules/ir_jam.h"
#include "../resources/info_texts.h"
#include <stdio.h>

static const char* ir_mode_labels[] = {
    "Noise Burst",
    "TV Codes",
    "Random RAW",
    "NEC Flood",
    "Samsung",
    "Sony",
};
static const char* ir_led_labels[] = {"Intern", "IR Blaster"};

/* ------------------------------------------------------------------ */
/*  Config                                                              */
/* ------------------------------------------------------------------ */

static void ir_mode_change(VariableItem* item) {
    JammerApp* app   = variable_item_get_context(item);
    uint8_t    index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, ir_mode_labels[index]);
    app->ir_mode = (IrMode)index;
}

static void ir_led_change(VariableItem* item) {
    JammerApp* app   = variable_item_get_context(item);
    uint8_t    index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, ir_led_labels[index]);
    app->ir_external = (index == 1);
}

typedef enum {
    IrCfgMode = 0,
    IrCfgLed,
    IrCfgDuration,
    IrCfgStart,
    IrCfgInfo,
} IrCfgIndex;

static void ir_config_enter(void* context, uint32_t index) {
    JammerApp* app = context;
    if(index == IrCfgDuration) {
        app->active_module = SceneIrRun;
        scene_manager_next_scene(app->scene_manager, SceneDuration);
    } else if(index == IrCfgStart) {
        scene_manager_next_scene(app->scene_manager, SceneIrRun);
    } else if(index == IrCfgInfo) {
        app->info_text = INFO_IR;
        scene_manager_next_scene(app->scene_manager, SceneInfo);
    }
}

void jammer_scene_IrConfig_on_enter(void* context) {
    JammerApp*    app = context;
    VariableItem* item;

    variable_item_list_reset(app->var_list);
    variable_item_list_set_header(app->var_list, "IR Konfiguration");

    item = variable_item_list_add(app->var_list, "Modus", 6, ir_mode_change, app);
    variable_item_set_current_value_index(item, (uint8_t)app->ir_mode);
    variable_item_set_current_value_text(item, ir_mode_labels[(uint8_t)app->ir_mode]);

    item = variable_item_list_add(app->var_list, "LED", 2, ir_led_change, app);
    variable_item_set_current_value_index(item, app->ir_external ? 1 : 0);
    variable_item_set_current_value_text(item, ir_led_labels[app->ir_external ? 1 : 0]);

    variable_item_list_add(app->var_list, "Dauer waehlen...", 1, NULL, app);
    variable_item_list_add(app->var_list, ">> START <<",      1, NULL, app);
    variable_item_list_add(app->var_list, "? Modul / Reichweite", 1, NULL, app);

    variable_item_list_set_enter_callback(app->var_list, ir_config_enter, app);
    view_dispatcher_switch_to_view(app->view_dispatcher, ViewVarList);
}

bool jammer_scene_IrConfig_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void jammer_scene_IrConfig_on_exit(void* context) {
    JammerApp* app = context;
    variable_item_list_reset(app->var_list);
}

/* ------------------------------------------------------------------ */
/*  Run Screen                                                          */
/* ------------------------------------------------------------------ */

static void ir_run_update_widget(JammerApp* app) {
    widget_reset(app->widget);

    const char* mode_str = ir_mode_labels[(uint8_t)app->ir_mode];
    const char* led_str  = ir_led_labels[app->ir_external ? 1 : 0];

    char line1[40];
    snprintf(line1, sizeof(line1), "Modus: %s", mode_str);
    char line2[32];
    snprintf(line2, sizeof(line2), "LED: %s", led_str);

    char line_time[32];
    if(app->total_ms == 0) {
        snprintf(line_time, sizeof(line_time), "Zeit: Unbegrenzt");
    } else {
        uint32_t remaining = (app->total_ms - app->elapsed_ms) / 1000;
        snprintf(line_time, sizeof(line_time), "Restzeit: %lu s", (unsigned long)remaining);
    }

    widget_add_string_element(app->widget, 64,  2, AlignCenter, AlignTop, FontPrimary,   "IR JAMMER AKTIV");
    widget_add_string_element(app->widget,  2, 18, AlignLeft,   AlignTop, FontSecondary, line1);
    widget_add_string_element(app->widget,  2, 30, AlignLeft,   AlignTop, FontSecondary, line2);
    widget_add_string_element(app->widget,  2, 42, AlignLeft,   AlignTop, FontSecondary, line_time);
    widget_add_string_element(app->widget, 64, 55, AlignCenter, AlignTop, FontSecondary, "[BACK] Stopp");
}

void jammer_scene_IrRun_on_enter(void* context) {
    JammerApp* app = context;

    app->elapsed_ms = 0;
    app->total_ms   = app->duration_ms;
    app->running    = true;

    ir_jam_start((IrJamMode)app->ir_mode, app->ir_external);
    jammer_notify_start(app);

    ir_run_update_widget(app);
    view_dispatcher_switch_to_view(app->view_dispatcher, ViewWidget);

    furi_timer_start(app->run_timer, 500);
}

bool jammer_scene_IrRun_on_event(void* context, SceneManagerEvent event) {
    JammerApp* app     = context;
    bool       handled = false;

    if(event.type == SceneManagerEventTypeCustom && event.event == 0xDEAD) {
        ir_jam_stop();
        jammer_notify_stop(app);
        scene_manager_previous_scene(app->scene_manager);
        handled = true;
    } else if(event.type == SceneManagerEventTypeTick) {
        ir_run_update_widget(app);
        handled = true;
    }
    return handled;
}

void jammer_scene_IrRun_on_exit(void* context) {
    JammerApp* app = context;
    furi_timer_stop(app->run_timer);
    app->running = false;
    ir_jam_stop();
    widget_reset(app->widget);
}
