#include "game.h"

#include <engine/eventmanager.h>
#include <engine/graphics.h>
#include <engine/mathext.h>

#include <stdlib.h>
#include <time.h>

#include "stage.h"
#include "player.h"
#include "mushroom.h"
#include "spikeball.h"
#include "coin.h"
#include "stats.h"
#include "message.h"
#include "enemy.h"
#include "pausemenu.h"

// Constants that are actually macros, d'oh!
#define MUSHROOM_COUNT 8
#define SPIKEBALL_COUNT 8
#define COIN_COUNT 64
#define MSG_COUNT 16
#define ENEMY_COUNT 16

// Constants
static const float MUSHROOM_GEN_TIME = 90.0f;
static const int ITEM_WAIT_MIN = 2;
static const int ITEM_WAIT_MAX = 6;
static const int MAX_PHASE = 4;
static const float INITIAL_MUSHROOM_WAIT = 60.0f;

// Probabilities & other phase-specific things
static const int LIFE_WAIT_MIN[] = {
    3, 4, 5, 6, 7
};
static const int LIFE_WAIT_MAX[] = {
    6, 8, 10, 12, 14
};
static const int MUSHROOM_PROB[][6] = {
    {35, 20, 30, 15, 0, 0},
    {30, 20, 25, 15, 10, 0},
    {25, 15, 20, 15, 15, 10},
    {25, 15, 20, 15, 15, 10},
    {25, 15, 20, 15, 15, 10},
};
static const int MINOR_PROB[][6] = {
    {10, 0, 10, 0, 0, 0},
    {10, 0, 10, 10, 0, 0},
    {10, 0, 10, 15, 0, 10},
    {10, 0, 10, 20, 0, 20},
    {10, 0, 10, 25, 0, 25},
};
static const int ENEMY_PROB[][5] = {
    {50, 50, 0, 0, 0},
    {30, 30, 20, 0, 20},
    {25, 25, 20, 10, 20},
    {20, 20, 20, 20, 20},
    {20, 20, 20, 20, 20},
};
static const int SPIKEBALL_MIN_TIME[] = {
    6, 5, 4, 3, 2
};
static const int SPIKEBALL_MAX_TIME[] = {
    12, 11, 10, 9, 8
};
static const int SPIKEBALL_SPECIAL_PROB[] = {
    0, 20, 30, 40, 50
};
static const int ENEMY_WAIT_MIN[] = {
    90, 75, 60, 60, 60,
};
static const int ENEMY_WAIT_MAX[] = {
    300, 300, 300, 270, 240
};
static const int ENEMY_CREATE_MAX[] = {
    1, 2, 2, 2, 3
};
// In seconds
static const float PHASE_LENGTH[] = {
    20, 20, 20, 20, 20
};


// Bitmaps
static Bitmap* bmpFont;
static Bitmap* bmpHUD;
static Bitmap* bmpNumbersBig;

// Components
static Stage stage;
static Player player;
static Mushroom mushrooms [MUSHROOM_COUNT];
static Spikeball spikeballs [SPIKEBALL_COUNT];
static Coin coins [COIN_COUNT];
static Message messages [MSG_COUNT];
static Enemy enemies [ENEMY_COUNT];
static Stats stats;

// Pause menu
static PauseMenu pause;

// Mushroom timer & stuff
static float mushroomTimer;
// Make sure a special type mushroom
// won't appear until this counter hits 0
static int prohibitSpecialCount;
// Spikeball wait counter
static int spikeballWait;
// Item counter
static int itemCounter;
// Life counter
static int lifeCounter;
// Enemy timer
static float enemyTimer;

// Global speed
static float globalSpeed;
// Speed target
static float globalSpeedTarget;
// Phase
static int phase;
// Phase timer
static float phaseTimer;
// "End phase", i.e. how many
// phases have been passed since reaching
// the maximum default phase
static int endPhase;

// Is paused
static int paused;
// Skip drawing
static bool skipDrawing;


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


// Generate a spikeball
static void gen_spikeball(float x, float y) {

    int i;
    Spikeball* b;
    for (i = 0; i < SPIKEBALL_COUNT; ++ i) {

        if (spikeballs[i].exist == false) {

            b = &spikeballs[i];
            break;
        }
    }
    if (b == NULL) return;

    // Create... err, activate
    sb_activate(b, x, y, 
       ((rand() % 100) < SPIKEBALL_SPECIAL_PROB[phase] ) ? 1 : 0 );
}


// Get next mushrom
static Mushroom* get_next_mushroom(int dir) {

    int i;
    int begin = 0;
    int end = MUSHROOM_COUNT;

    if (dir == -1) {

        begin = end-1;
        end = -1;
    }
    else 
        dir = 1;

    // Find the first mushroom that is not active
    for (i = begin; i != end; i += dir) {

        if (mushrooms[i].exist == false) {

            return &mushrooms[i];
        }
    }
    return NULL;
}


// Get next enemy
static Enemy* get_next_enemy() {

    int i;
    for (i = 0; i < ENEMY_COUNT; ++ i) {

        if (enemies[i].exist == false)
            return &enemies[i];
    }
    return NULL;
}


// Generate enemy id
static int gen_enemy_id(int prob) {

    int i;
    int p = ENEMY_PROB[phase][0];
    for (i = 1; i < 5; ++ i) {

        if (prob < p) {

            return i-1;
        }
        p += ENEMY_PROB[phase][i];
    }
    return 4;
}



// Create starter mushrooms
static void create_starter_mushrooms() {

    const int COUNT = 3;
    const int XPOS[] = {
        88, 168, 256,
    };
    const int ID[] = {
        1, 0, 0,
    };

    Mushroom* m;
    int i;
    for (i = 0; i < COUNT; ++ i) {

        m = get_next_mushroom(1);
        if (m == NULL) break;

        mush_activate(m, 
            vec2(XPOS[i], 192-GROUND_COLLISION_HEIGHT ),
            ID[i], 0);
    }
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
    const int SPIKEBALL_WAIT_MIN = 2;
    const int SPIKEBALL_WAIT_MAX = 10;
    // Needed when creating spikeballs
    const float MUSHROOM_MAX_JUMP = 64;
    const float MUSHROOM_MAX_FLY = 32;
    const int ITEM_Y_OFF = -16;
    const int COIN_PROB = 50;

    int i;
    int minor, major;
    float wait = 1.0f;
    Mushroom* m = NULL;

    int type;
    Coin* c = NULL;

    int dir = 1;
    float x, y;

    // Update & check the timer
    if ((mushroomTimer -= globalSpeed * tm) <= 0.0f) {

        // Reduce counters
        -- itemCounter;

        // Determine types
        major = get_index(rand() % 100);
        minor = get_minor_index(major, rand() % 100);

        if (major != 0 && major != 2 &&
            minor == 1) {

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
        }

        // Find the first mushroom that is not active
        m = get_next_mushroom(dir);
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

        // Determine positions for spikeballs & items
        y = m->pos.y-m->spr.height;
        // Jumping
        if (m->majorType == 3)
                y -= MUSHROOM_MAX_JUMP;
        // Flying & stationary
        else if (m->majorType == 4)
            y -= MUSHROOM_MAX_FLY;

        // Update spikeball counter
        if ((-- spikeballWait) <= 0) {

            // Generate a new spikeball
            gen_spikeball(256 + X_OFF, y);

            // Set a new wait time
            spikeballWait = (rand() % 
                (SPIKEBALL_MAX_TIME[phase]-SPIKEBALL_MIN_TIME[phase])) 
                + SPIKEBALL_MIN_TIME[phase];
        }
        else if (itemCounter <= 0) {

            c = coin_get_next(coins, COIN_COUNT);
            if (c == NULL) return;

            // Determine type
            if (-- lifeCounter <= 0) {

                type = 2;
                lifeCounter = LIFE_WAIT_MIN[phase]
                    + (rand() % (LIFE_WAIT_MAX[phase]-LIFE_WAIT_MIN[phase]));
            }
            else {

                type = (rand() % 100) <= COIN_PROB ? 0 : 1;
            }

            // Create coin
            y += ITEM_Y_OFF;
            coin_activate(c, vec2(256 + X_OFF, y), vec2(0, 0),
                type, true);

            // Set new item time
            if (type == 0)
                itemCounter = ITEM_WAIT_MIN;
            else
                itemCounter = ITEM_WAIT_MIN 
                    + (rand() % (ITEM_WAIT_MAX-ITEM_WAIT_MIN));
        }   
    }
}


// Update enemy generator
static void update_enemy_generator(float globalSpeed, float tm) {

    const int POS_X = 256 + 48;
    const int MIN_Y = 32;
    const int MAX_Y = 192-GROUND_COLLISION_HEIGHT -32;
    const int MAX_ID = 4;
    const int LOOP_OFF = 32;

    int loop;
    int i;
    int id;
    int startID;
    Vector2 pos;
    Enemy* e;
    int minTime;

    bool generated[] = {
        false, false, false, false, false
    };

    if ((enemyTimer -= 1.0f * tm) <= 0.0f) {

        loop = rand() % ENEMY_CREATE_MAX[phase] +1;

        // Compute position
        pos.x = POS_X;

        for (i = 0; i < loop; ++ i) {

            // Determine id
            id = gen_enemy_id(rand() % 100);
            // If id is already generated, get next
            while (generated[id] || 
                ENEMY_PROB[phase][id] == 0) {

                ++ id;
                id %= MAX_ID;
                if (id == startID) break;
            }
            generated[id] = true;

            pos.y = (float)(rand() % (MAX_Y-MIN_Y)) + MIN_Y;

            e = get_next_enemy();
            if (e == NULL) return;

            enemy_activate(e, pos, id);
            pos.x += LOOP_OFF;
        }

        minTime = ENEMY_WAIT_MIN[phase] * loop;
        // Compute new time
        enemyTimer = (float)(
            (rand() % (ENEMY_WAIT_MAX[phase] -ENEMY_WAIT_MIN[phase] ))
            +  minTime
        );

    }
}


// Update phase
static void update_phase(float tm) {

    const float SPEED_INCREASE = 0.25f;

    int target = PHASE_LENGTH[phase] * 60 * (endPhase +1);
    phaseTimer += 1.0f * tm;
    if (phaseTimer >= target) {

        phaseTimer -= target;
        globalSpeedTarget += SPEED_INCREASE;

        if (phase < MAX_PHASE)
            ++ phase;
        else 
            ++ endPhase;
    }
}


// Change scene to game over
static void go_game_over(void* e) {

    ev_change_scene((EventManager*)e, 
        "gameover", (void*)(size_t)stats.score);
}


// Defined ahead
static void game_reset();
// Trigger game over
static void trigger_game_over(EventManager* evMan) {

    tr_activate(evMan->tr, 
        FadeIn, EffectZoom, 1.0f, go_game_over, 0);
}


// Reset
static void game_reset() {

    // Create stats
    stats = create_default_stats();

    // Create components
    stage = create_stage();
    player = create_player(48, 48, &stats, 
        trigger_game_over);

    // Init arrays
    int i;
    for (i = 0; i < MUSHROOM_COUNT; ++ i) {

        mushrooms[i] = create_mushroom();
    }
    for (i = 0; i < SPIKEBALL_COUNT; ++ i) {

        spikeballs[i] = create_spikeball();
    }
    for (i = 0; i < COIN_COUNT; ++ i) {

        coins[i] = create_coin();
    }
    for (i = 0; i < MSG_COUNT; ++ i) {

        messages[i] = create_message();
    }
    for (i = 0; i < ENEMY_COUNT; ++ i) {

        enemies[i] = create_enemy();
    }

    // Set initials
    globalSpeed = 0.0f;
    globalSpeedTarget = 1.0f;
    mushroomTimer = INITIAL_MUSHROOM_WAIT;
    enemyTimer = (float)(
            (rand() % (ENEMY_WAIT_MAX[0] - ENEMY_WAIT_MIN[0]))
            +  ENEMY_WAIT_MIN[0]);
    prohibitSpecialCount = 0;
    phase = 0;
    spikeballWait = (rand() % 
                (SPIKEBALL_MAX_TIME[phase]-SPIKEBALL_MIN_TIME[phase])) 
                + SPIKEBALL_MIN_TIME[phase];
    paused = false;
    itemCounter = ITEM_WAIT_MIN 
        + (rand() % (ITEM_WAIT_MAX-ITEM_WAIT_MIN));
    lifeCounter = LIFE_WAIT_MIN[phase]
            + (rand() % (LIFE_WAIT_MAX[phase]-LIFE_WAIT_MIN[phase]));
    endPhase = 0;
    skipDrawing = false;

    // Create starter mushrooms
    create_starter_mushrooms();
}


// Initialize
static int game_init(void* e) {

    srand(time(NULL));

    // Create pause menu
    pause = create_pause_menu();

    return 0;
}


// On load
static int game_on_load(AssetManager* a) { 

    // Get bitmaps
    bmpFont = (Bitmap*)assets_get(a, "font");
    bmpHUD = (Bitmap*)assets_get(a, "hud");
    bmpNumbersBig = (Bitmap*)assets_get(a, "numbersBig");

    // Initialize global components
    init_global_stage(a);
    init_global_player(a);
    init_global_mushrooms(a);
    init_global_spikeballs(a);
    init_global_coins(a);
    init_global_enemies(a);

    // (Re)set
    game_reset();

    return 0;
}


// Update
static void game_update(void* e, float tm) {

    const float GSPEED_DELTA = 0.01f;
    // Needed to get proper movement speed
    // for the moving objects
    const float PERSPECTIVE_SPEED_MUL = 1.1f;

    EventManager* evMan = (EventManager*)e;

    skipDrawing = evMan->tr->active && evMan->tr->effect == EffectZoom;
    if (evMan->tr->active) return;

    int i, j;

    // Update pause menu
    if (pause.active) {

        pause_update(&pause, evMan);
        return;
    }

    // Pause
    if (pad_get_button_state(evMan->vpad, "start") == StatePressed) {

        pause_activate(&pause);
        return;
    }
    
    // Update phase
    update_phase(tm);

    // Update global speed
    if (globalSpeed < globalSpeedTarget) {

        globalSpeed += GSPEED_DELTA * tm;
        if (globalSpeed > globalSpeedTarget)
            globalSpeed = globalSpeedTarget;
    }
    float speed = globalSpeed * PERSPECTIVE_SPEED_MUL;

    // Update stage
    stage_update(&stage, globalSpeed, tm);

    // Update player
    pl_update(&player, evMan, speed, tm,    
        (void*)coins, COIN_COUNT,
        messages, MSG_COUNT);

    // Update enemy generator
    update_enemy_generator(speed, tm);
    // Update mushroom generator
    update_mushroom_generator(speed, tm);
    // Update mushrooms
    for (i = 0; i < MUSHROOM_COUNT; ++ i) {

        mush_update(&mushrooms[i], speed, tm);
        mush_player_collision(&mushrooms[i], &player,
            coins, COIN_COUNT,
            messages, MSG_COUNT);
    }
    // Update spikeballs
    for (i = 0; i < SPIKEBALL_COUNT; ++ i) {

        sb_update(&spikeballs[i], speed, tm);
        sb_player_collision(&spikeballs[i], &player);

        // Bullet collision
        for (j = 0; j < BULLET_COUNT; ++ j) {

            sb_bullet_collision(&spikeballs[i], 
                &player.bullets[j]);
        }
    }
    // Update enemies
    for (i = 0; i < ENEMY_COUNT; ++ i) {

        enemy_update(&enemies[i], speed, tm);
        enemy_player_collision(&enemies[i], &player,
            coins, COIN_COUNT,
            messages, MSG_COUNT);

        // Bullet collision
        for (j = 0; j < ENEMY_COUNT; ++ j) {

            enemy_bullet_collision(&enemies[i], 
                &player.bullets[j], &stats, 
                coins, COIN_COUNT,
                messages, MSG_COUNT);
        }
    }

    // Update coins
    for (i = 0; i < COIN_COUNT; ++ i) {

        coin_update(&coins[i], speed, tm);
        coin_player_collision(&coins[i], &player);
    }
    
    // Update messages
    for (i = 0; i < MSG_COUNT; ++ i) {

        msg_update(&messages[i], tm);
    }

    // Update stats
    stats_update(&stats, tm);
}


// Draw top-left hud portion
static void game_draw_top_left_hud(Graphics* g) {

    const int LIVES_X_OFF = 18;
    const int LIVES_X = 4;
    const int LIVES_Y = 4;

    const float PLAYER_LIMIT_X = 96;
    const float PLAYER_LIMIT_Y = 56;

    int i;
    int sx, sy;

    // Check if the player is too close the hud
    if (player.pos.x < PLAYER_LIMIT_X && 
        player.pos.y < PLAYER_LIMIT_Y) {

        g_set_pixel_function(g, 
            PixelFunctionSkipSimple, 0, 0);
    }

    // Draw lives
    for (i = 0; i < stats.maxLives; ++ i) {

        sx = stats.lives-1 >= i ? 0 : 16;
        g_draw_bitmap_region(g, bmpHUD, sx, 0, 16, 16,
            LIVES_X + i *LIVES_X_OFF, LIVES_Y,
            false);
    }

    g_set_pixel_function(g, PixelFunctionDefault, 0, 0);
}


// Draw score
static void game_draw_hud_score(Graphics* g) {

    const int SCORE_DELTA_X = 16;
    const int SCORE_TEXT_Y = 2;
    const int SCORE_Y_OFF = 8;
    const int SMALL_FONT_X_OFF = -1;
    const int BIG_FONT_X_OFF = -5;

    const float PLAYER_LIMIT_X = 32;
    const float PLAYER_LIMIT_Y = 56;

    // Check if the player is too close the text
    int mid = g->csize.x/2;
    if (player.pos.x > mid-PLAYER_LIMIT_X &&
        player.pos.x < mid+PLAYER_LIMIT_X && 
        player.pos.y < PLAYER_LIMIT_Y) {

        g_set_pixel_function(g, 
            PixelFunctionSkipSimple, 0, 0);
    }

    g_draw_text(g, bmpFont, "SCORE", 
        mid + SCORE_DELTA_X, 
        SCORE_TEXT_Y, 
        SMALL_FONT_X_OFF, 0, 
        true);

    g_draw_text(g, bmpNumbersBig, stats.scoreStr, 
        mid + SCORE_DELTA_X, 
        SCORE_TEXT_Y+SCORE_Y_OFF,
        BIG_FONT_X_OFF, 0, true);

    g_set_pixel_function(g, PixelFunctionDefault, 0, 0);
}


// Draw coins
static void game_draw_hud_coins(Graphics* g) {

    const int ICON_X = 200;
    const int ICON_Y = 4;

    const int TEXT_X = 216;
    const int TEXT_Y = 4;
    const int TEXT_X_OFF = -5;

    const float PLAYER_LIMIT_X = 192;
    const float PLAYER_LIMIT_Y = 48;

    if (player.pos.x > PLAYER_LIMIT_X &&
        player.pos.y < PLAYER_LIMIT_Y) {

        g_set_pixel_function(g, 
            PixelFunctionSkipSimple, 0, 0);
    }

    // Draw icon
    g_draw_bitmap_region(g, bmpHUD,
        48, 0, 16, 16, ICON_X, ICON_Y, false);

    // Draw text    
    char buf[5];
    snprintf(buf, 5, 
        stats.coins < 10 ? ":0%d" : ":%d", stats.coins);
    g_draw_text(g, bmpNumbersBig, buf, 
        TEXT_X, TEXT_Y, TEXT_X_OFF, 0, false);

    g_set_pixel_function(g, PixelFunctionDefault, 0, 0);
}


// Draw bottom bars
static void game_draw_bottom_bars(Graphics* g) {

    const int STAR_Y = 192-18;
    const int STAR_X = 256-92;
    const int STAR_BAR_X_OFF = 17;
    const int STAR_BAR_Y = STAR_Y+4;

    const int GEM_X = 4;
    const int GEM_Y = STAR_Y;

    const int POWER_BAR_X_OFF = 18;
    const int POWER_BAR_Y = STAR_BAR_Y;

    int i, sx, sy, sw;

    // Draw star 
    g_draw_bitmap_region(g, bmpHUD, 32, 0, 16, 16,
            STAR_X, STAR_Y, false);

    // Draw gem 
    g_draw_bitmap_region(g, bmpHUD, 64, 0, 16, 16,
            GEM_X, GEM_Y, false);

    // Draw star bar
    int level = (int)floorf(stats.powerMeterRenderPos);
    for (i = 0; i < 3; ++ i) {

        sx = i < level ? 50 : 25;

        g_draw_bitmap_region(g, bmpHUD, 
            sx, 16, 25, 10,
            STAR_X + STAR_BAR_X_OFF + i*24,
            STAR_BAR_Y,
            false);
    }
    if (level < 3) {

        sx = 0;
        sw = (int)(25.0f * (stats.powerMeterRenderPos - (float)level));

        g_draw_bitmap_region(g, bmpHUD, 
            sx, 16, sw, 10,
            STAR_X + STAR_BAR_X_OFF + level*24,
            STAR_BAR_Y,
            false);
    }

    // Draw power bar
    sx = (int)roundf(fabsf(stats.gunPowerRenderPos * 70));
    sy = stats.gunPowerRenderPos < 0.0f ? 46 : 26;

    // Back
    g_draw_bitmap_region(g, bmpHUD,
        0, 36, 72, 10, GEM_X + POWER_BAR_X_OFF,
        POWER_BAR_Y,
        false);

    // Front
    g_draw_bitmap_region(g, bmpHUD,
        0, sy, 2 + sx, 10, GEM_X + POWER_BAR_X_OFF,
        POWER_BAR_Y,
        false);

    // Draw bar end
    if (sx < 70) {

        g_fill_rect(g, 
            GEM_X + POWER_BAR_X_OFF + sx +1, 
            POWER_BAR_Y+1,
            1, 8, 0);
    }
}


// Draw hud
static void game_draw_hud(Graphics* g) {

    // Draw different portions
    game_draw_top_left_hud(g);
    game_draw_hud_score(g);
    game_draw_hud_coins(g);
    game_draw_bottom_bars(g);
}


// Draw self-destruct
static void game_draw_self_destruct(Graphics* g) {

    const int BOX_W = 120;
    const int BOX_H = 24;
    const int BOX_Y = 144;
    const int BOX_DVALUE = 5;

    const int TEXT_Y = 2;
    const int TEXT_XOFF = -1;

    const int ICON_Y = 12;
    const int ICON_X = 2;

    const int BAR_X = ICON_X + 12;
    const int BAR_Y = ICON_Y;
    const int BAR_WIDTH = 96;
    const int BAR_HEIGHT = 10;
    const uint8 BAR_COLORS[] = {
        0b01011110,
        0b10011001,
        0b11010000,
        0b11101000
    };

    int boxX = g->csize.x/2 - BOX_W/2;

    float t = player.selfDestructTimer;
    if (t <= 0.0f) return;
    t = 1.0f - t,

    // Draw box
    g_set_pixel_function(g, PixelFunctionDarken, BOX_DVALUE, 0);
    g_fill_rect(g, boxX, BOX_Y, BOX_W, BOX_H, 0);
    g_set_pixel_function(g, PixelFunctionDefault, 0, 0);

    // Draw text
    g_draw_text(g, bmpFont, "SELF-DESTRUCT IN", 
        g->csize.x/2, BOX_Y + TEXT_Y,
        TEXT_XOFF, 0, true);

    // Draw icon
    g_draw_bitmap_region(g, bmpHUD, 83, 3, 10, 10,
        boxX + ICON_X, BOX_Y + ICON_Y, false);

    // Draw bar
    uint8 col = (uint8)min_int32_2((int)floorf(4 * (1.0f-t)), 3);
    g_fill_rect(g, 
        boxX + BAR_X, BOX_Y + BAR_Y, (int)roundf(BAR_WIDTH * t),
        BAR_HEIGHT, BAR_COLORS[col]);

    // Create time string
    char timeStr[4];
    int s = (int)floorf( (1.0f-player.selfDestructTimer) * 10.0f);
    snprintf(timeStr, 4, "0.%d", s);
    
    g_draw_text(g, bmpFont, timeStr, 
        boxX + BAR_X + BAR_WIDTH/2, BOX_Y + BAR_Y +1,
        TEXT_XOFF, 0, true);
}


// Draw
static void game_draw(Graphics* g) {

    const int SHAKE_VARY = 4;

    g_move_to(g, 0, 0);    
    if (skipDrawing) return;

    // Draw pause
    if (pause.active) {

        pause_draw(&pause, g);
        return;
    }

    int i;

    // Shake!
    pl_shake(&player, g);

    // Draw stage
    stage_draw(&stage, g);

    // Draw spikeball shadows
    for (i = 0; i < SPIKEBALL_COUNT; ++ i) {

        sb_draw_shadow(&spikeballs[i], g);
    }

    // Draw player shadow
    pl_draw_shadow(&player, g);

    // Draw player entrance portal
    pl_draw_entrance_portal(&player, g);

    // Draw mushrooms
    for (i = 0; i < MUSHROOM_COUNT; ++ i) {

        mush_draw(&mushrooms[i], g);
    }

    // Draw spikeballs
    for (i = 0; i < SPIKEBALL_COUNT; ++ i) {

        sb_draw(&spikeballs[i], g);
    }

    // Draw enemies
    for (i = 0; i < ENEMY_COUNT; ++ i) {

        enemy_draw(&enemies[i], g);
    }

    // Draw player
    pl_draw(&player, g);
    // Draw explosion
    pl_draw_explosion(&player, g);

    // Remove shaking
    g_move_to(g, 0, 0);

    // Draw coins
    for (i = 0; i < COIN_COUNT; ++ i) {

        coin_draw(&coins[i], g);
    }

    // Draw messages
    for (i = 0; i < MSG_COUNT; ++ i) {

        msg_draw(&messages[i], g, bmpFont);
    }

    // Draw HUD
    game_draw_hud(g);

    // Draw self-destruct box
    game_draw_self_destruct(g);

}


// Dispose
static void game_dispose() {

    // ...
}


// On change
static void game_on_change(void* param) {

    if (param == NULL) {

        game_reset();
    }
}


// Get the game scene
Scene game_get_scene() {

    Scene s = (Scene) {
        game_init,
        game_on_load,
        game_update,
        game_draw,
        game_dispose,
        game_on_change
    };

    return s;
}
