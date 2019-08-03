#include "intro.h"

#include <engine/mathext.h>
#include <engine/eventmanager.h>

#include <math.h>
#include <stdlib.h>

// Constants
static const float INTRO_WAIT = 180;

// Bitmaps
static Bitmap* bmpIntro;
static Bitmap* bmpFace;

// Intro timer
static float introTimer;


// Callbacks
static void cb_go_to_title(void* e) {

    ev_change_scene((EventManager*)e,
        "title", NULL);
}


// Initialize
static int intro_init(void* e) {

    // Set defaults
    introTimer = INTRO_WAIT;

    return 0;
}


// On load
static int intro_on_load(AssetManager* a) { 

    // Get bitmaps
    bmpIntro = (Bitmap*)assets_get(a, "intro");
    bmpFace = (Bitmap*)assets_get(a, "face");
}


// Update
static void intro_update(void* e, float tm) {

    EventManager* evMan = (EventManager*)e;

    introTimer -= 1.0f * tm;
    if (evMan->tr->active == false &&
        introTimer <= 30.0f) {

        tr_activate(evMan->tr, FadeIn, EffectFade,
            2.0f, cb_go_to_title, 0);
    }
}


// Draw
static void intro_draw(Graphics* g) {

    const int TEXT_Y = 112;
    const int FACE_TARGET = 104;
    const int FACE_BEGIN = 0;
    const float BOUNCE_SCALE = 0.5f;
    
    g_clear_screen(g, 0);

    float t = 1.0f - fabsf(introTimer - INTRO_WAIT/2.0f) / 
        (INTRO_WAIT/2.0f);
    float s;
    int y = FACE_TARGET;
    int sx = bmpFace->width / 2;
    int sy = bmpFace->height / 2;

    // Draw face
    if (t < 0.33f) {

        y = (int)floorf( 
            FACE_BEGIN + (float)(FACE_TARGET-FACE_BEGIN) * t * 3.0f
            );
    }
    if(t > 0.33f && t <= 0.67f) {

        t -= 0.33f;
        s = sinf(M_PI * t * 3);

        sx = (int)(sx * (1.0f+s*BOUNCE_SCALE));
        sy = (int)(sy * (1.0f-s*BOUNCE_SCALE));
    }

    // Draw face
    g_draw_scaled_bitmap_region(g, bmpFace,
        0, 0, bmpFace->width, bmpFace->height,
        g->csize.x/2 - sx/2, y - sy,
        sx, sy, false);

    // Draw text
    g_draw_bitmap(g, bmpIntro, g->csize.x/2 - bmpIntro->width/2, 
        TEXT_Y, false);

}


// Dispose
static void intro_dispose() {


}


// Change
static void intro_on_change(void* param) {

}


// Get the leaderboard scene
Scene intro_get_scene() {

    Scene s = (Scene) {
        intro_init,
        intro_on_load,
        intro_update,
        intro_draw,
        intro_dispose,
        intro_on_change
    };

    return s;
}
