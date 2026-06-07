#include "../jammer_app.h"
#include "scenes.h"
#include "../modules/fuzzer.h"
#include <stdio.h>

/* ------------------------------------------------------------------ */
/*  RFID Fuzzer Run                                                     */
/* ------------------------------------------------------------------ */

static void rfid_fuzzer_update_widget(JammerApp* app) {
    widget_reset(app->widget);

    char line_iter[40];
    snprintf(line_iter, sizeof(line_iter), "IDs gesendet: %lu", (unsigned long)fuzzer_iteration_count());

    char line_time[32];
    if(app->total_ms == 0) {
        snprintf(line_time, sizeof(line_time), "Zeit: Unbegrenzt");
    } else {
        uint32_t remaining = (app->total_ms - app->elapsed_ms) / 1000;
        snprintf(line_time, sizeof(line_time), "Restzeit: %lu s", (unsigned long)remaining);
    }

    widget_add_string_element(app->widget, 64,  2, AlignCenter, AlignTop, FontPrimary,   "RFID FUZZER AKTIV");
    widget_add_string_element(app->widget,  2, 18, AlignLeft,   AlignTop, FontSecondary, "Zufaellige EM4100 IDs");
    widget_add_string_element(app->widget,  2, 30, AlignLeft,   AlignTop, FontSecondary, line_iter);
    widget_add_string_element(app->widget,  2, 42, AlignLeft,   AlignTop, FontSecondary, line_time);
    widget_add_string_element(app->widget, 64, 55, AlignCenter, AlignTop, FontSecondary, "[BACK] Stopp");
}

void jammer_scene_RfidFuzzerRun_on_enter(void* context) {
    JammerApp* app = context;

    app->elapsed_ms = 0;
    app->total_ms   = app->duration_ms;
    app->running    = true;

    rfid_fuzzer_start();
    jammer_notify_start(app);

    rfid_fuzzer_update_widget(app);
    view_dispatcher_switch_to_view(app->view_dispatcher, ViewWidget);

    app->tick_ms = 500;
    furi_timer_start(app->run_timer, app->tick_ms);
}

bool jammer_scene_RfidFuzzerRun_on_event(void* context, SceneManagerEvent event) {
    JammerApp* app     = context;
    bool       handled = false;

    if(event.type == SceneManagerEventTypeCustom && event.event == 0xDEAD) {
        rfid_fuzzer_stop();
        jammer_notify_stop(app);
        scene_manager_previous_scene(app->scene_manager);
        handled = true;
    } else if(event.type == SceneManagerEventTypeTick) {
        rfid_fuzzer_update_widget(app);
        handled = true;
    }
    return handled;
}

void jammer_scene_RfidFuzzerRun_on_exit(void* context) {
    JammerApp* app = context;
    furi_timer_stop(app->run_timer);
    app->running = false;
    rfid_fuzzer_stop();
    widget_reset(app->widget);
}
