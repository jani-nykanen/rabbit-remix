#include "game.h"

#include <engine/eventmanager.h>


// Initialize
static int game_init(void* e) {


    return 0;
}


// On load
static int game_on_load(AssetManager* a) { 

    return 0;
}


// Update
static void game_update(void* e, float tm) {

    EventManager* evMan = (EventManager*)e;
    if(evMan->tr->active) return;

}


// Draw
static void game_draw(Graphics* g) {
    
    g_clear_screen(g, 170, 170, 170);
}


// Dispose
static void game_dispose() {

    // ...
}


// Get the game scene
Scene game_get_scene() {

    Scene s = (Scene) {
        game_init,
        game_on_load,
        game_update,
        game_draw,
        game_dispose
    };

    return s;
}
