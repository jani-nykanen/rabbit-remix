#include "game.h"

#include <engine/eventmanager.h>
#include <engine/graphics.h>
#include <engine/mathext.h>

// Bitmaps
static Bitmap* bmpFont;

// Test angle
static float testAngle;


// Initialize
static int game_init(void* e) {
    
    testAngle = 0.0f;

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


    testAngle += 0.05f * tm;
    testAngle = fmodf(testAngle, M_PI*2.0f);
}


// Draw
static void game_draw(Graphics* g) {
    
    g_draw_static(g);

    g_fill_rect(g, 1, 1, 12*8, 10, 0b11100000);
    g_draw_text(g, bmpFont, "HELLO WORLD!", 2, 2, 0, 0, false);

    g_draw_bitmap_region_fast(g, bmpFont, 0, 0, 128, 64, 128, 96);


    // Draw a spinning triangle
    int cx = 128;
    int cy = 96;
    int r = 48 + abs(sinf(testAngle)*32);
    float step = M_PI*2.0f/3.0f;
    g_draw_triangle(g, 
    cx +  (int)(cosf(testAngle)*r), cy + (int)(sinf(testAngle)*r), 
    cx +  (int)(cosf(testAngle+step)*r), cy + (int)(sinf(testAngle+step)*r) , 
    cx +  (int)(cosf(testAngle+step*2)*r), cy + (int)(sinf(testAngle+step*2)*r) , 
    0b00011100);
    
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
