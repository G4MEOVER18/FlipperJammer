#include "../jammer_app.h"
#include "scenes.h"
#include "../modules/usb_fuzz.h"
#include <stdio.h>

static const char* usb_mode_labels[] = {
    "HID Flood",
    "Malformed HID",
    "Descriptor Fuzz",
    "Reconnect Loop",
};

/* ------------------------------------------------------------------ */
/*  Config                                                              */
/* ------------------------------------------------------------------ */

static void usb_mode_change(VariableItem* item) {
    JammerApp* app   = variable_item_get_context(item);
    uint8_t    index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, usb_mode_labels[index]);
    app->usb_mode = (UsbFuzzMode)index;
}

typedef enum {
    UsbCfgMode = 0,
    UsbCfgDuration,
    UsbCfgStart,
} UsbCfgIndex;

static void usb_config_enter(void* context, uint32_t index) {
    JammerApp* app = context;
    if(index == UsbCfgDuration) {
        app->active_module = SceneUsbFuzzerRun;
        scene_manager_next_scene(app->scene_manager, SceneDuration);
    } else if(index == UsbCfgStart) {
        scene_manager_next_scene(app->scene_manager, SceneUsbFuzzerRun);
    }
}

void jammer_scene_UsbFuzzerConfig_on_enter(void* context) {
    JammerApp*    app = context;
    VariableItem* item;

    variable_item_list_reset(app->var_list);
    variable_item_list_set_header(app->var_list, "USB Fuzzer Konfig");

    item = variable_item_list_add(app->var_list, "Modus", 4, usb_mode_change, app);
    variable_item_set_current_value_index(item, (uint8_t)app->usb_mode);
    variable_item_set_current_value_text(item, usb_mode_labels[(uint8_t)app->usb_mode]);

    variable_item_list_add(app->var_list, "Dauer waehlen...", 1, NULL, app);
    variable_item_list_add(app->var_list, ">> START <<",      1, NULL, app);

    variable_item_list_set_enter_callback(app->var_list, usb_config_enter, app);
    view_dispatcher_switch_to_view(app->view_dispatcher, ViewVarList);
}

bool jammer_scene_UsbFuzzerConfig_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void jammer_scene_UsbFuzzerConfig_on_exit(void* context) {
    JammerApp* app = context;
    variable_item_list_reset(app->var_list);
}

/* ------------------------------------------------------------------ */
/*  Run Screen                                                          */
/* ------------------------------------------------------------------ */

static void usb_run_update_widget(JammerApp* app) {
    widget_reset(app->widget);

    char line_time[32];
    if(app->total_ms == 0) {
        snprintf(line_time, sizeof(line_time), "Zeit: Unbegrenzt");
    } else {
        uint32_t remaining = (app->total_ms - app->elapsed_ms) / 1000;
        snprintf(line_time, sizeof(line_time), "Restzeit: %lu s", (unsigned long)remaining);
    }

    widget_add_string_element(app->widget, 64,  2, AlignCenter, AlignTop, FontPrimary,   "USB FUZZER AKTIV");
    widget_add_string_element(app->widget,  2, 18, AlignLeft,   AlignTop, FontSecondary, usb_mode_labels[(uint8_t)app->usb_mode]);
    widget_add_string_element(app->widget,  2, 30, AlignLeft,   AlignTop, FontSecondary, "USB HID Reports aktiv");
    widget_add_string_element(app->widget,  2, 42, AlignLeft,   AlignTop, FontSecondary, line_time);
    widget_add_string_element(app->widget, 64, 55, AlignCenter, AlignTop, FontSecondary, "[BACK] Stopp");
}

void jammer_scene_UsbFuzzerRun_on_enter(void* context) {
    JammerApp* app = context;

    app->elapsed_ms = 0;
    app->total_ms   = app->duration_ms;
    app->running    = true;

    usb_fuzz_start((UsbFuzzMode)app->usb_mode);
    jammer_notify_start(app);

    usb_run_update_widget(app);
    view_dispatcher_switch_to_view(app->view_dispatcher, ViewWidget);

    furi_timer_start(app->run_timer, 500);
}

bool jammer_scene_UsbFuzzerRun_on_event(void* context, SceneManagerEvent event) {
    JammerApp* app     = context;
    bool       handled = false;

    if(event.type == SceneManagerEventTypeCustom && event.event == 0xDEAD) {
        usb_fuzz_stop();
        jammer_notify_stop(app);
        scene_manager_previous_scene(app->scene_manager);
        handled = true;
    } else if(event.type == SceneManagerEventTypeTick) {
        usb_run_update_widget(app);
        handled = true;
    }
    return handled;
}

void jammer_scene_UsbFuzzerRun_on_exit(void* context) {
    JammerApp* app = context;
    furi_timer_stop(app->run_timer);
    app->running = false;
    usb_fuzz_stop();
    widget_reset(app->widget);
}
