#include "game.h"

#include <engine/eventmanager.h>
#include <engine/graphics.h>
#include <engine/mathext.h>

#include <stdlib.h>

#include "stage.h"
#include "player.h"
#include "mushroom.h"

#define MUSHROOM_COUNT 8

// Bitmaps
static Bitmap* bmpFont;

// Components
static Stage stage;
static Player player;
static Mushroom mushrooms [MUSHROOM_COUNT];

// Global speed
static float globalSpeed;


// Initialize
static int game_init(void* e) {
    
    return 0;
}


// On load
static int game_on_load(AssetManager* a) { 

    bmpFont = (Bitmap*)assets_get(a, "font");

    // Initialize global components
    init_global_player(a);
    init_global_mushrooms(a);

    // Create components
    stage = create_stage(a);
    player = create_player(64, 192-32);

    // Init arrays
    int i;
    for (i = 0; i < MUSHROOM_COUNT; ++ i) {

        mushrooms[i] = create_mushroom();
    }

    // Set initials
    globalSpeed = 1.0f;

    return 0;
}


// Update
static void game_update(void* e, float tm) {

    EventManager* evMan = (EventManager*)e;
    if (evMan->tr->active) return;

    int i;

    // Update stage
    stage_update(&stage, globalSpeed, tm);

    // Update player
    pl_update(&player, evMan, tm);

    // Update mushrooms
    for (i = 0; i < MUSHROOM_COUNT; ++ i) {

        mush_update(&mushrooms[i], globalSpeed, tm);
    }
}


// Draw
static void game_draw(Graphics* g) {
    
    g_clear_screen(g, 0b10110110);

    int i;

    // Draw stage
    stage_draw(&stage, g);

    // Update mushrooms
    for (i = 0; i < MUSHROOM_COUNT; ++ i) {

        mush_draw(&mushrooms[i], g);
    }

    // Draw player
    pl_draw(&player, g);

    g_draw_text(g, bmpFont, "ALPHA 0.0.5", 2, 2, 0, 0, false);
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
