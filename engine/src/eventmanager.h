//
// Event manager
// (c) 2019 Jani Nykänen
//


#ifndef __EVENT_MANAGER__
#define __EVENT_MANAGER__

#include "input.h"
#include "gamepad.h"
#include "scenemanager.h"
#include "assets.h"
#include "transition.h"

// Event manager type
typedef struct {

    void* core;
    Input* input;
    Gamepad* vpad;
    SceneManager* sceneMan;
    AssetManager* assets;
    Transition* tr;

} EventManager;

// Create an event manager
EventManager create_event_manager(
    void* core,
    Input* input, Gamepad* vpad,
    SceneManager* sceneMan, AssetManager* assets,
    Transition* tr
);

// Change the scene
void ev_change_scene(EventManager* evMan, 
    const char* sceneName, void* param);

// Terminate
void ev_terminate(EventManager* evMan);

// Toggle fullscreen
void ev_toggle_fullscreen(EventManager* evMan);

// Get framerate
int ev_get_framerate(EventManager* evMan);

// Set framerate
void ev_set_framerate(EventManager* evMan, int fps);

#endif // __EVENT_MANAGER__
