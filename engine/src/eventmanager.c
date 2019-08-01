#include "eventmanager.h"

#include "core.h"


// Create an event manager
EventManager create_event_manager(
    void* core,
    Input* input, Gamepad* vpad,
    SceneManager* sceneMan, AssetManager* assets,
    Transition* tr, AudioPlayer* audio) {

    EventManager evMan;

    evMan.core = core;
    evMan.input = input;
    evMan.vpad = vpad;
    evMan.sceneMan = sceneMan;
    evMan.assets = assets;
    evMan.tr = tr;
    evMan.audio = audio;

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


// Get framerate
int ev_get_framerate(EventManager* evMan) {

    return ((Core*)evMan->core)->frameRate;
}


// Set framerate
void ev_set_framerate(EventManager* evMan, int fps) {

    ((Core*)evMan->core)->frameRate = fps;
}
