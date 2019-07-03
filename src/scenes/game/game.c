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
    
    g_clear_screen(g, 0b10110110);

    g_fill_rect(g, 1, 1, 12*8, 10, 0b11100000);
    g_draw_text(g, bmpFont, "HELLO WORLD!", 2, 2, 0, 0, false);

    g_draw_bitmap_region_fast(g, bmpFont, 0, 0, 128, 64, 128, 96);


    // Draw a spinning triangle
    float r = 0.20f + abs(sinf(testAngle)*0.1f);
    float step = M_PI*2.0f/3.0f;
    
    float z = 0.5f + 0.1f * sinf(testAngle);
    g_draw_triangle_3D(g,
        vec3((cosf(testAngle)*r), (sinf(testAngle)*r), z), 
        vec3((cosf(testAngle+step)*r), (sinf(testAngle+step)*r), z), 
        vec3((cosf(testAngle+step*2)*r), (sinf(testAngle+step*2)*r), z), 
        0b00011100);

    g_draw_triangle_3D(g,
        vec3((cosf(-testAngle)*r), -0.15f+(sinf(-testAngle)*r), (0.8f-z)*2), 
        vec3((cosf(-testAngle+step)*r),-0.15f+ (sinf(-testAngle+step)*r), (0.8f-z)*2), 
        vec3((cosf(-testAngle+step*2)*r), -0.15f+(sinf(-testAngle+step*2)*r), (0.8f-z)*2), 
        0b11100000);

    g_draw_triangle_3D(g,
        vec3(-0.25f+(cosf(testAngle)*r), (sinf(testAngle)*r), 0.5f), 
        vec3(-0.25f+(cosf(testAngle+step)*r), (sinf(testAngle+step)*r),0.5f), 
        vec3(-0.25f+(cosf(testAngle+step*2)*r), (sinf(testAngle+step*2)*r), 0.5f), 
        0b00000011);

    g_draw_triangle_buffer(g);
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
