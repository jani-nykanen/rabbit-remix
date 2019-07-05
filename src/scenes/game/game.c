#include "game.h"

#include <engine/eventmanager.h>
#include <engine/graphics.h>
#include <engine/mathext.h>

#include <stdlib.h>

// Bitmaps
static Bitmap* bmpFont;

// Test mesh
static Mesh* mTest;

// Test angle
static float testAngle;


// Create a pyramid
static Mesh* create_pyramid() {

    const float VERTICES[] = {
        -1, -1, -1,
        1, -1, -1,
        1, -1, 1,
        -1, -1, 1,

        -1, 1, -1,
        1, 1, -1,
        1, 1, 1,
        -1, 1, 1,
    };

    const uint8 COLORS[] = {
        0b01011101,
        0b01011101,
        0b01011101,
        0b01011101,

        0b01011101,
        0b01011101,
        0b01011101,
        0b01011101,
    };

    // Normals are normalized automatically
    const float NORMALS[] = {

        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
    };

    const uint16 INDICES[] = {

        0, 1, 2,
        2, 3, 0,

        0, 3, 4,   
        4, 7, 3,   
    };

    Mesh* m = create_mesh(VERTICES, COLORS, NORMALS, INDICES,
        24, 8, 24, 12);

    return m;
}


// Initialize
static int game_init(void* e) {
    
    testAngle = 0.0f;

    // Create a test mesh
    mTest = create_pyramid();
    if (mTest == NULL) {

        return 1;
    }

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


    testAngle += 0.015f * tm;
    testAngle = fmodf(testAngle, M_PI*2.0f);
}


// Draw
static void game_draw(Graphics* g) {
    
    g_clear_screen(g, 0b10110110);

    g_draw_bitmap_region_fast(g, bmpFont, 0, 0, 128, 64, 128, 96);

    // Set perspective
    g_set_perspective(g, M_PI/3.0f, 0.025f, 100.0f);

    g_load_identity(g);

    float z = 3.0f + 0.5f * sinf(testAngle);
    g_translate_model(g, 0.0f, 0.0f, z);
    g_rotate_model(g, testAngle, 3.0f, 1.0f, 2.0f);
    g_scale_model(g, 0.5f, 0.5f, 0.5f);    

    // Lighting
    g_enable_lighting(g, 0.75f, vec3(0, 0, 1));

    // Draw a spinning triangle
    float r = 0.20f + abs(sinf(testAngle)*0.1f);
    float step = M_PI*2.0f/3.0f;
    
    g_draw_mesh(g, mTest);

    g_draw_triangle_buffer(g);

    g_fill_rect(g, 1, 1, 12*8, 10, 0b11100000);
    g_draw_text(g, bmpFont, "HELLO WORLD!", 2, 2, 0, 0, false);
}


// Dispose
static void game_dispose() {

    destroy_mesh(mTest);
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
