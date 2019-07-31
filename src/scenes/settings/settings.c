#include "settings.h"

#include <engine/mathext.h>
#include <engine/eventmanager.h>

#include <math.h>
#include <stdlib.h>

// Bitmaps
static Bitmap* bmpFont;
static Bitmap* bmpSettings;

// Was the previous scene the game scene
static bool prevSceneGame;

// Wave timer
static float waveTimer;

// Cursor position
static int cpos;
// Framerate
static int frameRate;


// Change back to game
static void change_back(void* e) {

    ev_change_scene((EventManager*)e, 
        "game", NULL);

    Transition* tr = ((EventManager*)e)->tr;
    tr->effect = EffectCircle;
    tr->speed = 1.0f;
}
// Go to the previous scene
static void go_back(void* e) {

    ev_change_scene((EventManager*)e, 
        prevSceneGame ? "game" : "title", 
        (void*)(size_t)1);
}


// Initialize
static int settings_init(void* e) {

    return 0;
}


// On load
static int settings_on_load(AssetManager* a) { 

    // Get bitmaps
    bmpFont = (Bitmap*)assets_get(a, "font");
    bmpSettings = (Bitmap*)assets_get(a, "settings");

    // Set initials
    waveTimer = 0.0f;
    cpos = 4;
}


// Update
static void settings_update(void* e, float tm) {

    const float WAVE_SPEED = 0.1f;
    const float EPS = 0.1f;

    EventManager* evMan = (EventManager*)e;

    // Get stuff
    frameRate = ev_get_framerate(evMan);

    if (evMan->tr->active) return;

    // Update waves
    waveTimer += WAVE_SPEED * tm;
    waveTimer = fmodf(waveTimer, M_PI * 2);

    // Check vertical movement
    float stickDelta = evMan->vpad->delta.y;
    float stickPos = evMan->vpad->stick.y;

    if (stickPos > EPS && stickDelta > EPS) {

        ++ cpos;
    }
    else if(stickPos < -EPS && stickDelta < -EPS) {

        -- cpos;
    }
    cpos = neg_mod(cpos, 5);

    // Check enter or fire1
    // Wait for enter or jump button
    if (pad_get_button_state(evMan->vpad, "fire1") == StatePressed ||
        pad_get_button_state(evMan->vpad, "start") == StatePressed) {

        switch (cpos)
        {

        // Full screen
        case 2:
            ev_toggle_fullscreen(evMan);
            break;

        // Change frame rate
        case 3:

            ev_set_framerate(evMan, frameRate != 60 ? 60 : 30);
            break;

        // Back
        case 4:
            tr_activate(evMan->tr, FadeIn, EffectFade, 2.0f,
                go_back, 0);
            break;
        
        default:
            break;
        }
    }
}


// Draw title
static void draw_title(Graphics* g) {

    const int POS_Y = 24;
    const int XOFF = -3;
    const float PERIOD = M_PI*2.0f / 8.0f;
    const float AMPLITUDE = 4.0f;

    const int SHADOW_DARKEN = 4;
    const int SHADOW_OFF_X = 1;
    const int SHADOW_OFF_Y = 2;

    int i;
    int midx = g->csize.x / 2;
    int dx = midx - (8+1) * (16+XOFF)/2;
    int step = (16+XOFF);

    // Shadows
    g_set_pixel_function(g, PixelFunctionDarken, SHADOW_DARKEN, 0);
    for (i = 0; i < 8; ++ i) {

        g_draw_bitmap_region(g, bmpSettings, 
            i*16, 0, 16, 16, 
            dx + step*i + SHADOW_OFF_X,
            POS_Y + 
                (int)(sinf(waveTimer + PERIOD*i) * AMPLITUDE) +
                SHADOW_OFF_Y, 
            false);
    }

    // Base
    g_set_pixel_function(g, PixelFunctionDefault, 0, 0);
    for (i = 0; i < 8; ++ i) {

        g_draw_bitmap_region(g, bmpSettings, 
            i*16, 0, 16, 16, 
            dx + step*i,
            POS_Y + (int)(sinf(waveTimer + PERIOD*i) * AMPLITUDE), 
            false);
    }
}


// Draw menu
static void draw_menu(Graphics* g) {

    const int POS_X = 56;
    const int START_Y = 72;
    const int Y_OFF = 16;

    char buf[32];

    // SFX
    g_draw_text(g, bmpFont, "SFX VOLUME:   100", 
        POS_X, START_Y, 0, 0, false);

    // Music
    g_draw_text(g, bmpFont, "MUSIC VOLUME: 100", 
        POS_X, START_Y + Y_OFF, 0, 0, false);

    // Toggle fullscreen
    g_draw_text(g, bmpFont, "TOGGLE FULLSCREEN", 
        POS_X, START_Y + Y_OFF*2, 0, 0, false);

    // Framerate
    snprintf(buf, 32, "FRAMERATE: %d FPS", frameRate);
    g_draw_text(g, bmpFont, buf, 
        POS_X, START_Y + Y_OFF*3, 0, 0, false);

    // Back
    g_draw_text(g, bmpFont, "BACK", 
        POS_X, START_Y + Y_OFF*5, -1, 0, false);  

    // Draw cursor
    g_draw_text(g, bmpFont, "\6", 
        POS_X - 8, 
        START_Y + Y_OFF * (cpos + cpos/4), 
        0, 0, false);
}


// Draw
static void settings_draw(Graphics* g) {
        
    g_clear_screen(g, 0b01010011);

    // Draw title
    draw_title(g);

    // Draw menu
    draw_menu(g);
}


// Dispose
static void settings_dispose() {

    // ...
}


// Change
static void settings_on_change(void* param) {

    prevSceneGame = (bool)(size_t)param;
    cpos = 4;
}


// Get the settings scene
Scene settings_get_scene() {

    Scene s = (Scene) {
        settings_init,
        settings_on_load,
        settings_update,
        settings_draw,
        settings_dispose,
        settings_on_change
    };

    return s;
}
