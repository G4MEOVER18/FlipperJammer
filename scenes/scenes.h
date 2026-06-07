#pragma once

#include <gui/scene_manager.h>
#include "../jammer_app.h"

/* Generate scene handler function declarations */
#define ADD_SCENE(prefix, name, id) \
    void prefix##_scene_##name##_on_enter(void* context); \
    bool prefix##_scene_##name##_on_event(void* context, SceneManagerEvent event); \
    void prefix##_scene_##name##_on_exit(void* context);
#include "scene_list.h"
#undef ADD_SCENE

/* Scene handlers array (defined in scenes.c) */
extern const SceneManagerHandlers jammer_scene_handlers;
