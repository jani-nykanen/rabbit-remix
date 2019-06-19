#include "eventmanager.h"

#include "core.h"


// Create an event manager
EventManager create_event_manager(
    void* core,
    Input* input, Gamepad* vpad,
    SceneManager* sceneMan, AssetManager* assets,
    Transition* tr) {

    EventManager evMan;

    evMan.core = core;
    evMan.input = input;
    evMan.vpad = vpad;
    evMan.sceneMan = sceneMan;
    evMan.assets = assets;
    evMan.tr = tr;

    return evMan;
}


// Change the scene
void ev_change_scene(EventManager* evMan, 
    const char* sceneName, void* param) {

    scenes_change(evMan->sceneMan, sceneName, param);
}


// Terminate
void ev_terminate(EventManager* evMan) {

    core_terminate((Core*)evMan->core);
}


// Toggle fullscreen
void ev_toggle_fullscreen(EventManager* evMan) {

    core_toggle_fullscreen((Core*)evMan->core);
}
