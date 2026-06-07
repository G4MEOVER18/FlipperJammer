#include "../jammer_app.h"
#include "scenes.h"
#include "../modules/subghz_jam.h"
#include "../resources/info_texts.h"
#include <stdio.h>

/* ------------------------------------------------------------------ */
/*  Config Screen                                                       */
/* ------------------------------------------------------------------ */

static const char* freq_labels[]   = {"EU 433 MHz", "EU 868 MHz", "US 315 MHz", "US 915 MHz"};
static const char* mode_labels[]   = {"Carrier", "Noise", "Sweep"};
static const char* source_labels[] = {"Intern", "REK5Lab"};

static void subghz_freq_change(VariableItem* item) {
    JammerApp* app   = variable_item_get_context(item);
    uint8_t    index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, freq_labels[index]);
    app->subghz_preset = (SubGhzPreset)index;
}

static void subghz_mode_change(VariableItem* item) {
    JammerApp* app   = variable_item_get_context(item);
    uint8_t    index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, mode_labels[index]);
    app->subghz_mode = (SubGhzMode)index;
}

static void subghz_source_change(VariableItem* item) {
    JammerApp* app   = variable_item_get_context(item);
    uint8_t    index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, source_labels[index]);
    app->subghz_external = (index == 1);
}

typedef enum {
    SubGhzCfgFreq = 0,
    SubGhzCfgMode,
    SubGhzCfgSource,
    SubGhzCfgDuration,
    SubGhzCfgStart,
} SubGhzCfgIndex;

static void subghz_config_enter(void* context, uint32_t index) {
    JammerApp* app = context;
    if(index == SubGhzCfgDuration) {
        app->active_module = SceneSubGhzRun;
        scene_manager_next_scene(app->scene_manager, SceneDuration);
    } else if(index == SubGhzCfgStart) {
        scene_manager_next_scene(app->scene_manager, SceneSubGhzRun);
    }
}

void jammer_scene_SubGhzConfig_on_enter(void* context) {
    JammerApp*    app = context;
    VariableItem* item;

    variable_item_list_reset(app->var_list);
    variable_item_list_set_header(app->var_list, "SubGHz Konfiguration");

    /* Frequenz */
    item = variable_item_list_add(app->var_list, "Frequenz", 4, subghz_freq_change, app);
    variable_item_set_current_value_index(item, (uint8_t)app->subghz_preset);
    variable_item_set_current_value_text(item, freq_labels[(uint8_t)app->subghz_preset]);

    /* Modus */
    item = variable_item_list_add(app->var_list, "Modus", 3, subghz_mode_change, app);
    variable_item_set_current_value_index(item, (uint8_t)app->subghz_mode);
    variable_item_set_current_value_text(item, mode_labels[(uint8_t)app->subghz_mode]);

    /* Quelle */
    item = variable_item_list_add(app->var_list, "Quelle", 2, subghz_source_change, app);
    variable_item_set_current_value_index(item, app->subghz_external ? 1 : 0);
    variable_item_set_current_value_text(item, source_labels[app->subghz_external ? 1 : 0]);

    /* Dauer */
    variable_item_list_add(app->var_list, "Dauer waehlen...", 1, NULL, app);

    /* Start */
    variable_item_list_add(app->var_list, ">> START <<", 1, NULL, app);

    variable_item_list_set_enter_callback(app->var_list, subghz_config_enter, app);

    view_dispatcher_switch_to_view(app->view_dispatcher, ViewVarList);
}

bool jammer_scene_SubGhzConfig_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void jammer_scene_SubGhzConfig_on_exit(void* context) {
    JammerApp* app = context;
    variable_item_list_reset(app->var_list);
}

/* ------------------------------------------------------------------ */
/*  Run Screen                                                          */
/* ------------------------------------------------------------------ */

static float subghz_preset_to_freq(SubGhzPreset preset) {
    switch(preset) {
    case SubGhzPreset433: return 433.92f;
    case SubGhzPreset868: return 868.35f;
    case SubGhzPreset315: return 315.00f;
    case SubGhzPreset915: return 915.00f;
    default:              return 433.92f;
    }
}

static void subghz_run_update_widget(JammerApp* app) {
    widget_reset(app->widget);

    const char* mode_str = mode_labels[(uint8_t)app->subghz_mode];
    const char* freq_str = freq_labels[(uint8_t)app->subghz_preset];
    const char* src_str  = app->subghz_external ? "REK5Lab" : "Intern";

    char header[32];
    snprintf(header, sizeof(header), "SubGHz AKTIV");

    char line1[48];
    snprintf(line1, sizeof(line1), "Freq: %s (%s)", freq_str, src_str);

    char line2[32];
    snprintf(line2, sizeof(line2), "Modus: %s", mode_str);

    char line3[32];
    if(app->total_ms == 0) {
        snprintf(line3, sizeof(line3), "Zeit: Unbegrenzt");
    } else {
        uint32_t remaining = (app->total_ms - app->elapsed_ms) / 1000;
        snprintf(line3, sizeof(line3), "Restzeit: %lu s", (unsigned long)remaining);
    }

    widget_add_string_element(app->widget, 64, 2,  AlignCenter, AlignTop, FontPrimary,   header);
    widget_add_string_element(app->widget, 2,  18, AlignLeft,   AlignTop, FontSecondary, line1);
    widget_add_string_element(app->widget, 2,  30, AlignLeft,   AlignTop, FontSecondary, line2);
    widget_add_string_element(app->widget, 2,  42, AlignLeft,   AlignTop, FontSecondary, line3);
    widget_add_string_element(app->widget, 64, 55, AlignCenter, AlignTop, FontSecondary, "[BACK] Stopp");
}

void jammer_scene_SubGhzRun_on_enter(void* context) {
    JammerApp* app = context;

    app->elapsed_ms = 0;
    app->total_ms   = app->duration_ms;
    app->running    = true;

    float freq = subghz_preset_to_freq(app->subghz_preset);
    subghz_jam_start(freq, (SubGhzJamMode)app->subghz_mode, app->subghz_external);
    jammer_notify_start(app);

    subghz_run_update_widget(app);
    view_dispatcher_switch_to_view(app->view_dispatcher, ViewWidget);

    furi_timer_start(app->run_timer, 500);
}

bool jammer_scene_SubGhzRun_on_event(void* context, SceneManagerEvent event) {
    JammerApp* app     = context;
    bool       handled = false;

    if(event.type == SceneManagerEventTypeCustom && event.event == 0xDEAD) {
        /* Timer expired */
        subghz_jam_stop();
        jammer_notify_stop(app);
        scene_manager_previous_scene(app->scene_manager);
        handled = true;
    } else if(event.type == SceneManagerEventTypeTick) {
        subghz_run_update_widget(app);
        handled = true;
    }
    return handled;
}

void jammer_scene_SubGhzRun_on_exit(void* context) {
    JammerApp* app = context;
    furi_timer_stop(app->run_timer);
    app->running = false;
    subghz_jam_stop();
    widget_reset(app->widget);
}
