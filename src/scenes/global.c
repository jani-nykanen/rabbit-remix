#include "global.h"

#include <engine/eventmanager.h>


// Initialize
static int global_init(void* e) {

    // ...
    return 0;
}


// On load
static int global_on_load(AssetManager* a) { 

    // ...
}


// Update
static void global_update(void* e, float tm) {

    EventManager* evMan = (EventManager*)e;

    // Terminate
    if(input_get_key_state(evMan->input, 
        (int)SDL_SCANCODE_LCTRL) == StateDown &&
        input_get_key_state(evMan->input, 
        (int)SDL_SCANCODE_Q) == StatePressed) {

        ev_terminate(evMan);
    }

    // Fullscreen
    if( (input_get_key_state(evMan->input, 
        (int)SDL_SCANCODE_LALT) == StateDown &&
        input_get_key_state(evMan->input, 
        (int)SDL_SCANCODE_RETURN) == StatePressed) ||
        input_get_key_state(evMan->input,
        SDL_SCANCODE_F4) == StatePressed  ) {

        ev_toggle_fullscreen(evMan);
    }
    
}


// Draw
static void global_draw(Graphics* g) {
    
    // ...
}


// Dispose
static void global_dispose() {

    // ...
}


// Get the global scene
Scene global_get_scene() {

    Scene s = (Scene) {
        global_init,
        global_on_load,
        global_update,
        global_draw,
        global_dispose
    };

    return s;
}
