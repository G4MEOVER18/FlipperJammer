#include "../jammer_app.h"
#include "scenes.h"

static const char* duration_labels[] = {
    "10 Sekunden",
    "30 Sekunden",
    "1 Minute",
    "5 Minuten",
    "10 Minuten",
    "Unbegrenzt",
};

static void duration_change_callback(VariableItem* item) {
    JammerApp* app    = variable_item_get_context(item);
    uint8_t    index  = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, duration_labels[index]);
    app->duration    = (DurationOption)index;
    app->duration_ms = jammer_duration_to_ms(app->duration);
}

static void duration_enter_callback(void* context, uint32_t index) {
    UNUSED(index);
    JammerApp* app = context;
    /* Navigate to the run scene that was stored by caller */
    scene_manager_next_scene(app->scene_manager, app->active_module);
}

void jammer_scene_Duration_on_enter(void* context) {
    JammerApp*    app  = context;
    VariableItem* item;

    variable_item_list_reset(app->var_list);
    variable_item_list_set_header(app->var_list, "Dauer waehlen");

    item = variable_item_list_add(
        app->var_list,
        "Dauer",
        6,
        duration_change_callback,
        app);
    variable_item_set_current_value_index(item, (uint8_t)app->duration);
    variable_item_set_current_value_text(item, duration_labels[(uint8_t)app->duration]);

    variable_item_list_set_enter_callback(app->var_list, duration_enter_callback, app);

    view_dispatcher_switch_to_view(app->view_dispatcher, ViewVarList);
}

bool jammer_scene_Duration_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void jammer_scene_Duration_on_exit(void* context) {
    JammerApp* app = context;
    variable_item_list_reset(app->var_list);
}
