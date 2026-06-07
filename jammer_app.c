#include "jammer_app.h"
#include "scenes/scenes.h"
#include <notification/notification.h>

/* ------------------------------------------------------------------ */
/*  Duration helper                                                     */
/* ------------------------------------------------------------------ */

uint32_t jammer_duration_to_ms(DurationOption opt) {
    switch(opt) {
    case Duration10s:        return 10000;
    case Duration30s:        return 30000;
    case Duration1m:         return 60000;
    case Duration5m:         return 300000;
    case Duration10m:        return 600000;
    case DurationUnlimited:  return 0;
    default:                 return 30000;
    }
}

/* ------------------------------------------------------------------ */
/*  Timer callback                                                      */
/* ------------------------------------------------------------------ */

void jammer_timer_callback(void* context) {
    JammerApp* app = context;

    if(app->total_ms == 0) {
        /* Unlimited — just tick elapsed */
        app->elapsed_ms += 500;
        return;
    }

    if(app->elapsed_ms + 500 >= app->total_ms) {
        app->elapsed_ms = app->total_ms;
        app->running    = false;
        furi_timer_stop(app->run_timer);
        /* Signal the view dispatcher to go back */
        view_dispatcher_send_custom_event(app->view_dispatcher, 0xDEAD);
    } else {
        app->elapsed_ms += 500;
    }
}

/* ------------------------------------------------------------------ */
/*  Notification helpers                                                */
/* ------------------------------------------------------------------ */

void jammer_notify_start(JammerApp* app) {
    NotificationApp* notif = furi_record_open(RECORD_NOTIFICATION);
    notification_message(notif, &sequence_blink_red_10);
    furi_record_close(RECORD_NOTIFICATION);
    (void)app;
}

void jammer_notify_stop(JammerApp* app) {
    NotificationApp* notif = furi_record_open(RECORD_NOTIFICATION);
    notification_message(notif, &sequence_single_vibro);
    furi_record_close(RECORD_NOTIFICATION);
    (void)app;
}

/* ------------------------------------------------------------------ */
/*  ViewDispatcher callback                                             */
/* ------------------------------------------------------------------ */

static bool jammer_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    JammerApp* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

static bool jammer_back_event_callback(void* context) {
    furi_assert(context);
    JammerApp* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

/* ------------------------------------------------------------------ */
/*  Alloc / Free                                                        */
/* ------------------------------------------------------------------ */

JammerApp* jammer_app_alloc(void) {
    JammerApp* app = malloc(sizeof(JammerApp));
    memset(app, 0, sizeof(JammerApp));

    /* Default config */
    app->subghz_mode     = SubGhzModeCarrier;
    app->subghz_preset   = SubGhzPreset433;
    app->subghz_external = false;
    app->ir_mode         = IrModeNoiseBurst;
    app->ir_external     = false;
    app->nrf24_mode      = Nrf24ModeAll;
    app->wifi_channel    = 0; /* ALL */
    app->wifi_broadcast  = true;
    snprintf(app->wifi_bssid, sizeof(app->wifi_bssid), "FF:FF:FF:FF:FF:FF");
    app->usb_mode        = UsbFuzzHidFlood;
    app->duration        = Duration30s;
    app->duration_ms     = 30000;
    app->active_module   = SceneSubGhzRun;

    /* GUI */
    app->gui = furi_record_open(RECORD_GUI);

    /* Scene Manager */
    app->scene_manager = scene_manager_alloc(&jammer_scene_handlers, app);

    /* View Dispatcher */
    app->view_dispatcher = view_dispatcher_alloc();
    // view_dispatcher_enable_queue() ist deprecated seit API 87.1 → entfernt
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(app->view_dispatcher, jammer_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(app->view_dispatcher, jammer_back_event_callback);

    /* Views */
    app->submenu  = submenu_alloc();
    app->var_list = variable_item_list_alloc();
    app->widget   = widget_alloc();

    view_dispatcher_add_view(
        app->view_dispatcher, ViewSubmenu, submenu_get_view(app->submenu));
    view_dispatcher_add_view(
        app->view_dispatcher, ViewVarList, variable_item_list_get_view(app->var_list));
    view_dispatcher_add_view(
        app->view_dispatcher, ViewWidget, widget_get_view(app->widget));

    /* Attach dispatcher to GUI */
    view_dispatcher_attach_to_gui(
        app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    /* Timer (500 ms tick) */
    app->run_timer = furi_timer_alloc(jammer_timer_callback, FuriTimerTypePeriodic, app);

    return app;
}

void jammer_app_free(JammerApp* app) {
    furi_assert(app);

    /* Stop timer if running */
    if(app->running) {
        furi_timer_stop(app->run_timer);
        app->running = false;
    }
    furi_timer_free(app->run_timer);

    /* Remove views */
    view_dispatcher_remove_view(app->view_dispatcher, ViewSubmenu);
    view_dispatcher_remove_view(app->view_dispatcher, ViewVarList);
    view_dispatcher_remove_view(app->view_dispatcher, ViewWidget);

    submenu_free(app->submenu);
    variable_item_list_free(app->var_list);
    widget_free(app->widget);

    view_dispatcher_free(app->view_dispatcher);
    scene_manager_free(app->scene_manager);

    furi_record_close(RECORD_GUI);

    free(app);
}

/* ------------------------------------------------------------------ */
/*  Entry point                                                         */
/* ------------------------------------------------------------------ */

int32_t jammer_app_main(void* p) {
    UNUSED(p);
    JammerApp* app = jammer_app_alloc();

    scene_manager_next_scene(app->scene_manager, SceneMainMenu);
    view_dispatcher_run(app->view_dispatcher);

    jammer_app_free(app);
    return 0;
}
