#include "gameover.h"

#include "../../util.h"
#include "../../menu.h"

#include <engine/eventmanager.h>
#include <leaderboard/leaderboard.h>

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

// Constants & macros
#define SCORE_STRING_SIZE 8

// Bitmaps
static Bitmap* bmpGameOver;
static Bitmap* bmpFont;
static Bitmap* bmpNumbersBig;

// Scale multiplier
static float scaleMul;
static bool fadingOut;

// Score
static int score;
// Score entry
static Entry scoreEntry;

// Menu
static Menu menu;

// Name input
static char nameInput [LB_NAME_LENGTH];
static int charPointer;
// Is name box active
static bool nameBoxActive;


// Draw name box
static void draw_name_box(Graphics* g) {

    const uint8 BOX_COLOR[] = {
        255, 0, 83
    };
    const int BOX_OUTLINES = 3;
    const int BOX_W = 144;
    const int BOX_H = 32;
    const int TITLE_Y_OFF = 4;
    const int NAME_OFF = 12;

    int x = g->csize.x/2 - BOX_W/2;
    int y = g->csize.y/2 - BOX_H/2;
    int i;

    // Draw box
    for (i = 0; i < BOX_OUTLINES; ++ i) {

        g_fill_rect(g, 
            x - (BOX_OUTLINES-1 -i), 
            y - (BOX_OUTLINES-1 -i), 
            BOX_W + (BOX_OUTLINES-1 -i)*2, 
            BOX_H + (BOX_OUTLINES-1 -i)*2, 
            BOX_COLOR[i]);
    }

    // Draw title
    g_draw_text(g, bmpFont, "ENTER YOUR NAME:", 
        g->csize.x/2, y + TITLE_Y_OFF, 0, 0, true);

    // Draw name
    g_draw_text(g, bmpFont, nameInput, 
        g->csize.x/2, 
        y + TITLE_Y_OFF + NAME_OFF, 
        0, 0, true);
}


// Draw game over text
static void draw_game_over_text(Graphics* g) {

    const int POS_Y = 8;
    const float SCALE_PLUS = 0.5f;

    int midx = g->csize.x / 2;
    int midy = POS_Y + bmpGameOver->height/2;

    int sx = bmpGameOver->width;
    int sy = bmpGameOver->height;

    float t;

    if (scaleMul <= 0.0f || !fadingOut) {

        // Draw Game Over! bitmap, no
        // scaling
        g_draw_bitmap(g, bmpGameOver, 
            midx-bmpGameOver->width/2,
            POS_Y, false);

    }
    else {

        t = (1.0f+scaleMul*SCALE_PLUS);

        sx = (int)(sx * t);
        sy = (int)(sy * t);

        g_draw_scaled_bitmap_region(g, bmpGameOver,
            0, 0, bmpGameOver->width, bmpGameOver->height,
            midx - sx/2, midy - sy/2, sx, sy, false);
    }
}


// Draw info text
static void draw_info_text(Graphics* g) {

    const int POS_Y = 120;
    const int SCORE_OFF = 8;
    const int MENU_X = 72;
    const int MENU_OFF = 20;

    char scoreStr [6 +1];
    get_score_string(scoreStr, score, 6);

    // Draw score
    g_draw_text(g, bmpFont, "SCORE", 
        g->csize.x/2, POS_Y, -1, 0, true);
    g_draw_text(g, bmpNumbersBig, scoreStr, 
        g->csize.x/2, POS_Y + SCORE_OFF, -5, 0, true);

    // Draw menu
    menu_draw(&menu, g, MENU_X,
        POS_Y + SCORE_OFF + MENU_OFF);
}


// Change back to game
static void change_back(void* e) {

    ev_change_scene((EventManager*)e, 
        "game", NULL);

    Transition* tr = ((EventManager*)e)->tr;
    tr->effect = EffectCircle;
    tr->speed = 1.0f;
}
// Go to the title screen
static void go_to_title(void* e) {

    ev_change_scene((EventManager*)e, "title", NULL);
}


// Button callbacks
static void cb_submit_score(EventManager* evMan) {

    // ev_change_scene(evMan, "leaderboard", (void*)&scoreEntry);

    nameBoxActive = true;
}
static void cb_play_again(EventManager* evMan) {

    tr_activate(evMan->tr, FadeIn, EffectFade, 2.0f,
            change_back, 0);
}
static void cb_quit(EventManager* evMan) {

    tr_activate(evMan->tr, FadeIn, EffectFade, 2.0f,
            go_to_title, 0);
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
    fadingOut = false;
    nameBoxActive = false;
    nameInput[0] = '\0';
    charPointer = 0;

    // Create menu
    menu = create_menu();
    menu_add_button(&menu, cb_submit_score, "SUBMIT SCORE");
    menu_add_button(&menu, cb_play_again, "PLAY AGAIN");
    menu_add_button(&menu, cb_quit, "QUIT");
}


// Update
static void gover_update(void* e, float tm) {

    EventManager* evMan = (EventManager*)e;

    int i;

    if (evMan->tr->active) {

        fadingOut = evMan->tr->mode == FadeOut;
        scaleMul = tr_get_scaled_time(evMan->tr);

        return;
    }
    else {

        scaleMul = 0.0f;
    }

    // Update name box, if active
    if (nameBoxActive) {

        // Wait for characters
        if (charPointer < LB_NAME_LENGTH -1) {

            for (i = (int)SDL_SCANCODE_A; i <= (int)SDL_SCANCODE_0; ++ i) {

                if (input_get_key_state(evMan->input, i) == StatePressed) {

                    if (i == (int)SDL_SCANCODE_0) {

                        nameInput[charPointer ++] = '0';
                    }
                    else if (i >= (int)SDL_SCANCODE_1) {

                        nameInput[charPointer ++] = '1' + (i - (int)SDL_SCANCODE_1);
                    }
                    else {

                        nameInput[charPointer ++] = 'A' + (i - (int)SDL_SCANCODE_A);
                    }
                    break;
                }
            }
        }
        
        // Backspace
        if (charPointer > 0 && 
            input_get_key_state(evMan->input, (int)SDL_SCANCODE_BACKSPACE) 
            == StatePressed) {
            
            nameInput[-- charPointer] = '\0';
        }

        // Start & escape
        if (charPointer > 0 &&
            pad_get_button_state(evMan->vpad, "start") == StatePressed) {

            snprintf(scoreEntry.name, LB_NAME_LENGTH, "%s", nameInput);
            scoreEntry.score = score;
            ev_change_scene(evMan, "leaderboard", (void*)&scoreEntry);
        }
        else if (pad_get_button_state(evMan->vpad, "cancel") 
            == StatePressed) {

            nameBoxActive = false;
        }
    }
    else {

        // Update menu
        menu_update(&menu, evMan);
    }
}


// Draw
static void gover_draw(Graphics* g) {
        
    // Clear to black
    g_clear_screen(g, 0);
    
    // Draw Game Over! text
    draw_game_over_text(g);
    // Draw info text
    draw_info_text(g);

    // Draw name box
    if (nameBoxActive) {

        draw_name_box(g);
    }
}


// Dispose
static void gover_dispose(void* e) {

    // ...
}


// Change
static void gover_on_change(void* param) {

    score = (int)(size_t)param;
    menu.cpos = 0;
    nameBoxActive = false;
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
