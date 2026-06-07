#include "../jammer_app.h"
#include "scenes.h"
#include "../modules/nfc_jam.h"
#include <stdio.h>

/* ------------------------------------------------------------------ */
/*  NFC Jam Run                                                         */
/* ------------------------------------------------------------------ */

static void nfc_jam_update_widget(JammerApp* app) {
    widget_reset(app->widget);

    char line_time[32];
    if(app->total_ms == 0) {
        snprintf(line_time, sizeof(line_time), "Zeit: Unbegrenzt");
    } else {
        uint32_t remaining = (app->total_ms - app->elapsed_ms) / 1000;
        snprintf(line_time, sizeof(line_time), "Restzeit: %lu s", (unsigned long)remaining);
    }

    widget_add_string_element(app->widget, 64,  2, AlignCenter, AlignTop, FontPrimary,   "NFC JAMMER AKTIV");
    widget_add_string_element(app->widget,  2, 18, AlignLeft,   AlignTop, FontSecondary, "13.56 MHz Feld aktiv");
    widget_add_string_element(app->widget,  2, 30, AlignLeft,   AlignTop, FontSecondary, "ISO14443 / MIFARE block");
    widget_add_string_element(app->widget,  2, 42, AlignLeft,   AlignTop, FontSecondary, line_time);
    widget_add_string_element(app->widget, 64, 55, AlignCenter, AlignTop, FontSecondary, "[BACK] Stopp");
}

void jammer_scene_NfcJamRun_on_enter(void* context) {
    JammerApp* app = context;

    app->elapsed_ms = 0;
    app->total_ms   = app->duration_ms;
    app->running    = true;

    nfc_jam_start();
    jammer_notify_start(app);

    nfc_jam_update_widget(app);
    view_dispatcher_switch_to_view(app->view_dispatcher, ViewWidget);

    furi_timer_start(app->run_timer, 500);
}

bool jammer_scene_NfcJamRun_on_event(void* context, SceneManagerEvent event) {
    JammerApp* app     = context;
    bool       handled = false;

    if(event.type == SceneManagerEventTypeCustom && event.event == 0xDEAD) {
        nfc_jam_stop();
        jammer_notify_stop(app);
        scene_manager_previous_scene(app->scene_manager);
        handled = true;
    } else if(event.type == SceneManagerEventTypeTick) {
        nfc_jam_update_widget(app);
        handled = true;
    }
    return handled;
}

void jammer_scene_NfcJamRun_on_exit(void* context) {
    JammerApp* app = context;
    furi_timer_stop(app->run_timer);
    app->running = false;
    nfc_jam_stop();
    widget_reset(app->widget);
}
