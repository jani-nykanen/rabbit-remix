#include "gameover.h"

#include "../../util.h"
#include "../../menu.h"

#include <engine/eventmanager.h>

// Constants & macros
#define SCORE_STRING_SIZE 8

// Bitmaps
static Bitmap* bmpGameOver;
static Bitmap* bmpFont;
static Bitmap* bmpNumbersBig;

// Scale multiplier
static float scaleMul;

// Score
static int score;

// Menu
static Menu menu;


// Draw game over text
static void draw_game_over_text(Graphics* g) {

    const int POS_Y = 16;
    const float SCALE_PLUS = 0.5f;

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


// Draw info text
static void draw_info_text(Graphics* g) {

    const int POS_Y = 112;
    const int MOVE_Y = 192-POS_Y;
    const int SCORE_OFF = 8;
    const int MENU_X = 72;
    const int MENU_OFF = 28;

    int y = (int)(POS_Y + MOVE_Y*scaleMul);

    char scoreStr [6 +1];
    get_score_string(scoreStr, score, 6);

    // Draw score
    g_draw_text(g, bmpFont, "SCORE", 
        g->csize.x/2, y, -1, 0, true);
    g_draw_text(g, bmpNumbersBig, scoreStr, 
        g->csize.x/2, y + SCORE_OFF, -5, 0, true);

    // Draw menu
    menu_draw(&menu, g, MENU_X,
        y + SCORE_OFF + MENU_OFF);
}


// Change back to game
static void change_back(void* e) {

    ev_change_scene((EventManager*)e, 
        "game", NULL);
}


// Button callbacks
static void cb_play_again(EventManager* evMan) {

    tr_activate(evMan->tr, FadeIn, EffectFade, 1.0f,
            change_back, 0);
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

    // Create menu
    menu = create_menu();
    menu_add_button(&menu, NULL, "SUBMIT SCORE");
    menu_add_button(&menu, cb_play_again, "PLAY AGAIN");
    menu_add_button(&menu, NULL, "QUIT");
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

    // Update menu
    menu_update(&menu, evMan);
}


// Draw
static void gover_draw(Graphics* g) {
        
    // Clear to black
    g_clear_screen(g, 0);
    
    // Draw Game Over! text
    draw_game_over_text(g);
    // Draw info text
    draw_info_text(g);
}


// Dispose
static void gover_dispose() {

    // ...
}


// Change
static void gover_on_change(void* param) {

    score = (int)(size_t)param;
    menu.cpos = 0;
}


// Get the gover scene
Scene gover_get_scene() {

    Scene s = (Scene) {
        gover_init,
        gover_on_load,
        gover_update,
        gover_draw,
        gover_dispose,
        gover_on_change
    };

    return s;
}
