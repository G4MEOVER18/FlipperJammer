#include "scenes.h"

/* Neue SceneManagerHandlers API: drei separate Callback-Arrays */

#define ADD_SCENE(prefix, name, id) prefix##_scene_##name##_on_enter,
static const AppSceneOnEnterCallback jammer_on_enter_handlers[] = {
#include "scene_list.h"
};
#undef ADD_SCENE

#define ADD_SCENE(prefix, name, id) prefix##_scene_##name##_on_event,
static const AppSceneOnEventCallback jammer_on_event_handlers[] = {
#include "scene_list.h"
};
#undef ADD_SCENE

#define ADD_SCENE(prefix, name, id) prefix##_scene_##name##_on_exit,
static const AppSceneOnExitCallback jammer_on_exit_handlers[] = {
#include "scene_list.h"
};
#undef ADD_SCENE

const SceneManagerHandlers jammer_scene_handlers = {
    .on_enter_handlers = jammer_on_enter_handlers,
    .on_event_handlers = jammer_on_event_handlers,
    .on_exit_handlers  = jammer_on_exit_handlers,
    .scene_num         = SceneCount,
};
