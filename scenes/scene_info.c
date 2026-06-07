#include "../jammer_app.h"
#include "scenes.h"

void jammer_scene_Info_on_enter(void* context) {
    JammerApp* app = context;
    widget_reset(app->widget);

    widget_add_string_multiline_element(
        app->widget,
        4, 4,
        AlignLeft, AlignTop,
        FontSecondary,
        app->info_text ? app->info_text : "Kein Hilfetext verfuegbar.");

    view_dispatcher_switch_to_view(app->view_dispatcher, ViewWidget);
}

bool jammer_scene_Info_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void jammer_scene_Info_on_exit(void* context) {
    JammerApp* app = context;
    widget_reset(app->widget);
}
