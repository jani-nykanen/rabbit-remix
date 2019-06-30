#include "game.h"

#include <engine/eventmanager.h>
#include <engine/graphics.h>

// Bitmaps
static Bitmap* bmpFont;


// Initialize
static int game_init(void* e) {
    

    return 0;
}


// On load
static int game_on_load(AssetManager* a) { 

    bmpFont = (Bitmap*)assets_get(a, "font");

    return 0;
}


// Update
static void game_update(void* e, float tm) {

    EventManager* evMan = (EventManager*)e;
    if (evMan->tr->active) return;

}


// Draw
static void game_draw(Graphics* g) {
    
    g_draw_static(g);

    g_draw_text(g, bmpFont, "HELLO WORLD!", 2, 2, 0, 0, false);
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
