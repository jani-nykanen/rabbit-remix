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

// Bitmaps
static Bitmap* bmpFont;

// Components
static Stage stage;
static Player player;
static Mushroom mushrooms [MUSHROOM_COUNT];

// Mushroom timer & stuff
static float mushroomTimer;

// Global speed
static float globalSpeed;


// Update mushroom generator
static void update_mushroom_generator(float globalSpeed, float tm) {

    const float X_OFF = 64;
    const int TIME_VARY_MIN = -30;
    const int TIME_VARY_MAX = 90;

    int i;
    Mushroom* m = NULL;

    // Update & check the timer
    if ((mushroomTimer -= globalSpeed * tm) <= 0.0f) {

        // Find the first mushroom that is not active
        for (i = 0; i < MUSHROOM_COUNT; ++ i) {

            if (mushrooms[i].exist == false) {

                m = &mushrooms[i];
                break;
            }
        }
        if (m == NULL) return;

        // Create the mushroom to the screen
        mush_activate(m, vec2(256+X_OFF, 192-GROUND_COLLISION_HEIGHT), 0, 0);

        mushroomTimer += MUSHROOM_GEN_TIME +
            (float) ( (rand() % (TIME_VARY_MAX-TIME_VARY_MIN)) + TIME_VARY_MIN);
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

    // Update mushrooms
    for (i = 0; i < MUSHROOM_COUNT; ++ i) {

        mush_draw(&mushrooms[i], g);
    }

    // Draw player
    pl_draw(&player, g);

    g_draw_text(g, bmpFont, "ALPHA 0.0.9", 2, 2, 0, 0, false);
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
