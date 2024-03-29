#include "title.h"

#include "../../menu.h"

#include <engine/eventmanager.h>

#include <math.h>
#include <stdlib.h>

// Constants
static const int MIDDLE_X_OFF = 0;
static const int MIDDLE_Y_OFF = 72;
static const float ENTER_TIME = 60.0f;

// Bitmaps
static Bitmap* bmpFont;
static Bitmap* bmpLogo;
// Samples
static Sample* sStart;
static Sample* sReject;

// Menu
static Menu menu;

// Spiral angle
static float spiralAngle;

// Phase
static int phase;
// Enter timer
static float enterTimer;


// Go to the game scene
static void go_to_game(void* e) {

    ev_change_scene((EventManager*)e, 
        "game", NULL);

    Transition* tr = ((EventManager*)e)->tr;
    tr->effect = EffectCircle;
    tr->speed = 1.0f;
}
// Go to the settings
static void go_to_settings(void* e) {

    ev_change_scene((EventManager*)e, 
        "settings", (void*)(size_t) false);
}
// Terminate
static void terminate(void* e) {
    
    ev_terminate((EventManager*)e);
} 


// Button callbacks
static void cb_go_to_game(EventManager* evMan) {

    tr_activate(evMan->tr, FadeIn, EffectZoom, 1.0f,
            go_to_game, 0);
}
static void cb_go_to_leaderboard(EventManager* evMan) {

    ev_change_scene(evMan, "leaderboard", NULL);
}
static void cb_go_to_settings(EventManager* evMan) {

    tr_activate(evMan->tr, FadeIn, EffectFade, 2.0f,
            go_to_settings, 0);
}
static void cb_terminate(EventManager* evMan) {

    tr_activate(evMan->tr, FadeIn, EffectFade, 2.0f,
            terminate, 0);
}



// Initialize
static int title_init(void* e) {

    return 0;
}


// On load
static int title_on_load(AssetManager* a) { 

    // Get bitmaps
    bmpFont = (Bitmap*)assets_get(a, "font");
    bmpLogo = (Bitmap*)assets_get(a, "logo");

    // Get samples
    sStart = (Sample*)assets_get(a, "start");
    sReject = (Sample*)assets_get(a, "reject");

    // Create menu
    menu = create_menu();
    menu_add_button(&menu, cb_go_to_game, "START GAME");
    menu_add_button(&menu, cb_go_to_leaderboard, "LEADERBOARD");
    menu_add_button(&menu, cb_go_to_settings, "SETTINGS");
    menu_add_button(&menu, cb_terminate, "QUIT");

    // Set defaults
    spiralAngle = 0.0f;
    enterTimer = ENTER_TIME;
}


// Update
static void title_update(void* e, float tm) {

    const float SPIRAL_SPEED = 0.025f;

    EventManager* evMan = (EventManager*)e;

    if (evMan->tr->active) return;

    if (phase == 0) {

        if (pad_get_button_state(evMan->vpad, "start") == StatePressed ||
            pad_get_button_state(evMan->vpad, "fire1") == StatePressed) {

            ++ phase;

            audio_play_sample(evMan->audio, sStart, 0.70f, 0);
        }

        // Update enter timer
        enterTimer += 1.0f * tm;
        enterTimer = fmodf(enterTimer, ENTER_TIME);
    }
    else {

        // Update menu
        menu_update(&menu, evMan);
    }

    // Quit, if esc pressed
    if (pad_get_button_state(evMan->vpad, "cancel") == StatePressed) {

        audio_play_sample(evMan->audio, sReject, 0.70f, 0);

        cb_terminate(evMan);
    }

    // Update spiral
    spiralAngle += SPIRAL_SPEED * tm;
    spiralAngle = fmodf(spiralAngle, 2 * M_PI);
}


// Draw background
static void title_draw_background(Graphics* g) {

    const int COUNT = 10;
    const float RADIUS = 256.0f;
    const uint8 COLORS[] = {
        0b01000010,
        0b10000111
    };

    int cx = g->csize.x/2 + MIDDLE_X_OFF;
    int cy = MIDDLE_Y_OFF;

    int i;
    float step = M_PI * 2.0f / COUNT;

    for (i = 0; i < COUNT; ++ i) {

        g_draw_triangle(
            g,
            cx + (int)(cosf(-spiralAngle - step*i) * RADIUS),
            cy + (int)(sinf(-spiralAngle - step*i) * RADIUS),
            cx + (int)(cosf(-spiralAngle - step*(i+1)) * RADIUS),
            cy + (int)(sinf(-spiralAngle - step*(i+1)) * RADIUS),
            cx,
            cy,
            COLORS[i % 2]
        );
    }
}


// Draw
static void title_draw(Graphics* g) {
        
    const int POS_Y = 
        MIDDLE_Y_OFF + bmpLogo->height/2 - 12;
    const int MENU_X = 72;
    const int MENU_OFF = 20;
    const int COPYRIGHT_OFF = -10;
    const int ENTER_Y = POS_Y + MENU_OFF + 24;

    const float WAVE_MUL = 2.0f;
    const float PERIOD = 64;
    const float AMPLITUDE = 8.0f;

    // Clear to black
    g_clear_screen(g, 0);

    // Draw background
    title_draw_background(g);

    // Draw logo
    g_draw_waving_bitmap(g, bmpLogo, 
        g->csize.x/2 - bmpLogo->width/2 + MIDDLE_X_OFF,
        MIDDLE_Y_OFF - bmpLogo->height/2, 
        spiralAngle * WAVE_MUL,
        PERIOD, AMPLITUDE);

    if (phase == 0) {

        if (enterTimer <= ENTER_TIME / 2.0f) {

            // Draw "Press enter" text
            g_draw_text(g, bmpFont,
                "PRESS ENTER OR \7\10",
                g->csize.x/2, ENTER_Y,
                0, 0, true);
        }
    }
    else {

        // Draw menu
        menu_draw(&menu, g, MENU_X,
            POS_Y + MENU_OFF);
    }

    // Draw copyright
    g_draw_text(g, bmpFont, "\4 2019 JANI NYK\5NEN",
        g->csize.x/2, 
        g->csize.y + COPYRIGHT_OFF, 
        -1, 0, true);
}


// Dispose
static void title_dispose(void* e) {

    // ...
}


// Change
static void title_on_change(void* param) {

    
}


// Get the gover scene
Scene title_get_scene() {

    Scene s = (Scene) {
        title_init,
        title_on_load,
        title_update,
        title_draw,
        title_dispose,
        title_on_change
    };

    return s;
}
