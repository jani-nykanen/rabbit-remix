#include "gameover.h"

#include <engine/eventmanager.h>

// Bitmaps
static Bitmap* bmpGameOver;
static Bitmap* bmpFont;
static Bitmap* bmpNumbersBig;

// Scale multiplier
static float scaleMul;

// Score
static int score;


// Draw game over text
static void draw_game_over_text(Graphics* g) {

    const int POS_Y = 32;
    const float SCALE_PLUS = 1.0f;

    int midx = g->csize.x / 2;
    int midy = POS_Y + bmpGameOver->height/2;

    int sx = bmpGameOver->width;
    int sy = bmpGameOver->height;

    if (scaleMul <= 0.0f) {

        // Draw Game Over! bitmap, no
        // scaling
        g_draw_bitmap(g, bmpGameOver, 
            midx-bmpGameOver->width/2,
            POS_Y, false);

    }
    else {

        sx = (int)(sx * (1.0f+scaleMul*SCALE_PLUS));
        sy = (int)(sy * (1.0f+scaleMul*SCALE_PLUS));

        g_draw_scaled_bitmap_region(g, bmpGameOver,
            0, 0, bmpGameOver->width, bmpGameOver->height,
            midx - sx/2, midy - sy/2, sx, sy, false);
    }
}


// Change back to game
static void change_back(void* e) {

    ev_change_scene((EventManager*)e, 
        "game", NULL);
}


// Initialize
static int gover_init(void* e) {


    return 0;
}


// On load
static int gover_on_load(AssetManager* a) { 

    // Get bitmaps
    bmpFont = (Bitmap*)assets_get(a, "font");
    bmpGameOver = (Bitmap*)assets_get(a, "gameover");
    bmpNumbersBig = (Bitmap*)assets_get(a, "numbersBig");

    // Set defaults
    scaleMul = 0.0f;
    score = 0;
}


// Update
static void gover_update(void* e, float tm) {

    EventManager* evMan = (EventManager*)e;

    if (evMan->tr->active) {

        scaleMul = tr_get_scaled_time(evMan->tr);
        return;
    }
    else {

        scaleMul = 0.0f;
    }

    // Wait for enter or jump button
    if (pad_get_button_state(evMan->vpad, "fire1") == StatePressed ||
        pad_get_button_state(evMan->vpad, "start") == StatePressed) {

        tr_activate(evMan->tr, FadeIn, EffectFade, 1.0f,
            change_back, 0);
    }
}


// Draw
static void gover_draw(Graphics* g) {
        
    // Clear to black
    g_clear_screen(g, 0);
    
    // Draw Game Over! text
    draw_game_over_text(g);
}


// Dispose
static void gover_dispose() {

    // ...
}


// Change
static void gover_on_change(void* param) {

    score = (int)(size_t)param;
}


// Get the gover scene
Scene gover_get_scene() {

    Scene s = (Scene) {
        gover_init,
        gover_on_load,
        gover_update,
        gover_draw,
        gover_dispose
    };

    return s;
}
