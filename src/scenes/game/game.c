#include "game.h"

#include <engine/eventmanager.h>
#include <engine/graphics.h>
#include <engine/mathext.h>

#include <stdlib.h>

#include "stage.h"

// Bitmaps
static Bitmap* bmpFont;

// Components
static Stage stage;

// Global speed
static float globalSpeed;


// Initialize
static int game_init(void* e) {
    
    return 0;
}


// On load
static int game_on_load(AssetManager* a) { 

    bmpFont = (Bitmap*)assets_get(a, "font");

    // Create components
    stage = create_stage(a);

    // Set initials
    globalSpeed = 1.0f;

    return 0;
}


// Update
static void game_update(void* e, float tm) {

    EventManager* evMan = (EventManager*)e;
    if (evMan->tr->active) return;

    // Update stage
    stage_update(&stage, globalSpeed, tm);
}


// Draw
static void game_draw(Graphics* g) {
    
    g_clear_screen(g, 0b10110110);

    // Draw stage
    stage_draw(&stage, g);

    g_fill_rect(g, 1, 1, 12*8, 10, 0b11100000);
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
