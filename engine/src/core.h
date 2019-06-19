//
// Application core
// (c) 2019 Jani Nykänen
//

#ifndef __CORE__
#define __CORE__

#include "eventmanager.h"
#include "scenemanager.h"
#include "graphics.h"
#include "input.h"
#include "gamepad.h"
#include "config.h"
#include "assets.h"
#include "transition.h"

#include <SDL2/SDL.h>

#include <stdbool.h>

// Core type
typedef struct {

    // SDL components
    SDL_Window* window;

    // Components
    EventManager evMan;
    InputManager input;
    Gamepad vpad;
    SceneManager sceneMan;
    Graphics* g;
    AssetManager* assets;
    Transition tr;

    // Configuration
    Config conf;

    // Boolean flags
    bool running;
    bool fullscreen;

} Core;

// Run
void core_run(Core* c);

// Toggle fullscreen
void core_toggle_fullscreen(Core* c);

// Terminate
void core_terminate(Core* c);

// Create a core
Core* create_core();

#endif // __CORE__
