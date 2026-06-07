#include "../jammer_app.h"
#include "scenes.h"

/* Info-Scene mit scrollbarem TextBox.
   Up/Down scrollt durch laengeren Text, Back kehrt zurueck. */

void jammer_scene_Info_on_enter(void* context) {
    JammerApp* app = context;

    text_box_reset(app->text_box);
    text_box_set_font(app->text_box, TextBoxFontText);
    text_box_set_text(
        app->text_box,
        app->info_text ? app->info_text : "Kein Hilfetext verfuegbar.");

    view_dispatcher_switch_to_view(app->view_dispatcher, ViewTextBox);
}

bool jammer_scene_Info_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void jammer_scene_Info_on_exit(void* context) {
    JammerApp* app = context;
    text_box_reset(app->text_box);
}
