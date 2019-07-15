#include "game.h"

#include <engine/eventmanager.h>
#include <engine/graphics.h>
#include <engine/mathext.h>

#include <stdlib.h>
#include <time.h>

#include "stage.h"
#include "player.h"
#include "mushroom.h"

#define MUSHROOM_COUNT 8

// Constants
static const float MUSHROOM_GEN_TIME = 90.0f;

// Probabilities
static const int MUSHROOM_PROB[][6] = {
    {25, 15, 20, 15, 15, 10}
};
static const int MINOR_PROB[][6] = {
    {10, 0, 10, 25, 0, 25}
};


// Bitmaps
static Bitmap* bmpFont;

// Components
static Stage stage;
static Player player;
static Mushroom mushrooms [MUSHROOM_COUNT];

// Mushroom timer & stuff
static float mushroomTimer;
// Make sure a special type mushroom
// won't appear until this counter hits 0
static int prohibitSpecialCount;

// Global speed
static float globalSpeed;
// Phase
static int phase;


// Get index by probability
static int get_index(int prob) {

    int i;
    int p = MUSHROOM_PROB[phase][0];
    for (i = 1; i < 6; ++ i) {

        if (prob < p) {

            return i-1;
        }
        p += MUSHROOM_PROB[phase][i];
    }
    return 5;
}


// Get minor index
static int get_minor_index(int major, int prob) {

    return prob < MINOR_PROB[phase][major] ? 1 : 0;
}


// Update mushroom generator
static void update_mushroom_generator(float globalSpeed, float tm) {

    const float X_OFF = 64;
    const int TIME_VARY_MIN = -30;
    const int TIME_VARY_MAX = 60;
    const int MAJOR_MAX = 6;
    // To make sure two jumping mushrooms etc.
    // spawn at the same time
    const int PROHIBIT_WAIT = 3;

    int i;
    int minor, major;
    float wait = 1.0f;
    Mushroom* m = NULL;

    int dir = 1;
    int end = MUSHROOM_COUNT-1;
    int begin = 0;

    float x;

    // Update & check the timer
    if ((mushroomTimer -= globalSpeed * tm) <= 0.0f) {

        // Determine types
        major = get_index(rand() % 100);
        minor = get_minor_index(major, rand() % 100);

        if (minor == 1) {

            if (prohibitSpecialCount > 0) {

                -- prohibitSpecialCount;
                minor = 0;
            }
            else {

                prohibitSpecialCount = PROHIBIT_WAIT;
            }
        }

        // If flying forward, make sure it is drawn last
        if (major == 5 || (major == 3 && minor == 1)) {

            dir = -1;
            begin = end;
            end = 0;
        }

        // Find the first mushroom that is not active
        for (i = begin; i != end; i += dir) {

            if (mushrooms[i].exist == false) {

                m = &mushrooms[i];
                break;
            }
        }
        if (m == NULL) return;

        // Determine start pos
        x = 256+X_OFF;
        if ( (major == 5 || major == 3) && minor == 1) {

            x -= (256 + X_OFF*2);
        }

        // Create the mushroom to the screen
        wait = mush_activate(m, vec2(x, 
            192-GROUND_COLLISION_HEIGHT), 
            major, minor);

        // Special case: forward jumping
        // mushroom, no wait time
        mushroomTimer += max_float_2(0.0f, wait * MUSHROOM_GEN_TIME +
            (float) ( (rand() % (TIME_VARY_MAX-TIME_VARY_MIN)) 
            + TIME_VARY_MIN));
    }
}


// Initialize
static int game_init(void* e) {

    srand(time(NULL));

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
    mushroomTimer = 0.0f;
    prohibitSpecialCount = 0;
    phase = 0;

    return 0;
}


// Update
static void game_update(void* e, float tm) {

    // Needed to get proper movement speed
    // for the moving objects
    const float PERSPECTIVE_SPEED_MUL = 1.1f;

    EventManager* evMan = (EventManager*)e;
    if (evMan->tr->active) return;

    int i;
    float speed = globalSpeed * PERSPECTIVE_SPEED_MUL;

    // Update stage
    stage_update(&stage, globalSpeed, tm);

    // Update player
    pl_update(&player, evMan, tm);

    // Update mushroom generator
    update_mushroom_generator(speed, tm);
    // Update mushrooms
    for (i = 0; i < MUSHROOM_COUNT; ++ i) {

        mush_update(&mushrooms[i], speed, tm);
        mush_player_collision(&mushrooms[i], &player);
    }
}


// Draw
static void game_draw(Graphics* g) {
    
    g_clear_screen(g, 0b10110110);

    int i;

    // Draw stage
    stage_draw(&stage, g);

    // Draw player shadow
    pl_draw_shadow(&player, g);

    // Draw "normal" mushrooms
    for (i = 0; i < MUSHROOM_COUNT; ++ i) {

        if (mushrooms[i].majorType != 5)
            mush_draw(&mushrooms[i], g);
    }

    // Draw flying mushrooms
    for (i = 0; i < MUSHROOM_COUNT; ++ i) {

        if (mushrooms[i].majorType == 5)
            mush_draw(&mushrooms[i], g);
    }

    // Draw player
    pl_draw(&player, g);

    g_draw_text(g, bmpFont, "ALPHA 0.1.2", 2, 2, 0, 0, false);
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
