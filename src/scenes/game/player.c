#include "player.h"

#include <engine/mathext.h>

#include "stage.h"
#include "coin.h"

// Shared pointers to bitmaps
static Bitmap* bmpBunny;
static Bitmap* bmpBlast;
static Bitmap* bmpArrow;

// Constants
static const float JUMP_REACT_MIN_TIME = 1.0f;
static const float JUMP_EXTEND_TIME = 10.0f;
static const float DUST_WAIT = 8.0f;
static const float BLAST_TIME = 10.0f;
static const float GRAVITY_TARGET = 3.0f;
static const float RESPAWN_TIME = 30.0f;


// Init global data
void init_global_player(AssetManager* a) {

    bmpBunny = (Bitmap*)assets_get(a, "bunny");
    bmpBlast = (Bitmap*)assets_get(a, "blast");
    bmpArrow = (Bitmap*)assets_get(a, "arrow");
}


// Update axis
static void update_axis(float* axis, 
    float* speed, float target, float delta, float tm) {

    if (*speed < target) {

        *speed += delta * tm;
        if (*speed > target)
            *speed = target;
    }
    else if (*speed > target) {

        *speed -= delta * tm;
        if (*speed < target)
            *speed = target;
    }

    *axis += *speed * tm;
}


// Create coins
static void pl_create_coins(Player* pl, 
    Coin* coins, int len, int min, int max,
    int gemMax, bool makeLife, float globalSpeed) {

    const float SPEED_VARY_X_RIGHT = 2.5f;
    const float SPEED_VARY_X_LEFT = -1.5f;
    const float SPEED_VARY_Y_UP = -3.0f;
    const float SPEED_VARY_Y_DOWN = 0.0f;
    const float Y_OFF = 0.0f;
    const int GEM_PROB = 50;
    const float LIFE_SPEED_Y = -2.5f;

    int i;
    int loop = (rand() % (max-min)) + min;

    Vector2 speed;
    Vector2 pos = pl->pos;
    pos.y += Y_OFF;
    Coin* c;
    int type;

    for (i = 0 ; i < loop; ++ i) {

        // Get the next coin
        c = coin_get_next(coins, len);
        if (c == NULL) break;

        speed.x = (float)(rand() % 100)/100.0f * 
            (SPEED_VARY_X_RIGHT - SPEED_VARY_X_LEFT) + SPEED_VARY_X_LEFT ;
        speed.y = (float)(rand() % 100)/100.0f * 
            (SPEED_VARY_Y_DOWN - SPEED_VARY_Y_UP) + SPEED_VARY_Y_UP ;

        // Determine type
        type = 0;
        if (gemMax > 0 && (rand() % 100) >= GEM_PROB) {

            type = 1;
            -- gemMax;
        }

        // Activate
        coin_activate(c, pos, speed, type, false);
    }

    // Create life
    if (makeLife) {

        // Get the next coin
        c = coin_get_next(coins, len);
        if (c == NULL) return;

        // Activate
        coin_activate(c, pos, 
            vec2(globalSpeed, LIFE_SPEED_Y), 
            2, false);
    }
}


// Create an explosion
static void pl_create_explosion(Player* pl) {

    exp_activate(&pl->exp,
        vec2(pl->pos.x, pl->pos.y-pl->spr.height/2),
        pl->stats->powerLevel+1);
}


// Control
static void pl_control(Player* pl, EventManager* evMan, float tm,
    Coin* coins, int coinLen, float globalSpeed) {

    const float MOVE_TARGET = 1.5f;
    const float FLAP_SPEED = 0.5f;
    const float DJUMP_TIME = 7.0f;

    const float QUICK_FALL_EPS = 0.5f;
    const float QUICK_FALL_MUL = 1.5f;

    const float SELF_DESTRUCT_SPEED = 1.0f / 60.0f;

    const int GEM_MAX[] = {
        0, 0, 1, 2
    };
    const int COIN_MIN[] = {
        2, 3, 4, 5
    };
    const int COIN_MAX[] = {
        3, 5, 6, 8
    };

    // Set target speed
    pl->target.x = evMan->vpad->stick.x * MOVE_TARGET;
    pl->target.y = GRAVITY_TARGET;
    
    // Quick fall
    if (!pl->quickFall &&
        evMan->vpad->stick.y > QUICK_FALL_EPS &&
        evMan->vpad->delta.y > 0.0f) {

        pl->quickFall = true;
        pl->speed.y = pl->target.y * QUICK_FALL_MUL;
        pl->jumpTimer = 0.0f;
    }
    if (evMan->vpad->stick.y <= QUICK_FALL_EPS) {

        pl->quickFall = false;
    }
    if (pl->quickFall) {

        pl->target.y *= QUICK_FALL_MUL;
    }

    // Jumping
    State fire1 = pad_get_button_state(evMan->vpad, "fire1");
    if (!pl->extendJump && 
        pl->jumpTimer > JUMP_REACT_MIN_TIME && pl->doubleJump && 
        (fire1 == StatePressed || fire1 == StateDown)) {

        pl->jumpTimer += JUMP_EXTEND_TIME;
        pl->extendJump = true;
    }
    else  if (fire1 == StatePressed && pl->doubleJump) {
            
        pl->jumpTimer = DJUMP_TIME;
        pl->doubleJump = false;

        pl->jumpSpeedMul = 1.0f;
    }

    // Stop jumping
    if (fire1 == StateReleased && pl->jumpTimer > 0.0f) {

        if (!pl->doubleJump) {

            pl->djumpReleased = true;
        }

        pl->jumpTimer = 0.0f;
    }

    // Flapping
    pl->flapping = (pl->speed.y > 0 || pl->djumpReleased) && fire1 == StateDown;
    if (pl->flapping) {

        pl->speed.y = FLAP_SPEED;
        pl->target.y = pl->speed.y;
    }

    // Self-destruct
    int level = pl->stats->powerLevel;
    if (pad_get_button_state(evMan->vpad, "fire3") == StateDown) {

        pl->selfDestructTimer += SELF_DESTRUCT_SPEED * tm;
        if (pl->selfDestructTimer >= 1.0f) {

            // Spawn some coins
            pl_create_coins(pl, coins, coinLen, 
                COIN_MIN[level], COIN_MAX[level],
                GEM_MAX[level], level == 3,
                globalSpeed);

            // Create explosion and die
            pl_create_explosion(pl);
            pl_kill(pl, 1);
        }
    }
    else {

        pl->selfDestructTimer = 0.0f;
    }
}


// Move
static void pl_move(Player* pl, EventManager* evMan, float tm) {

    const float ACC_X = 0.20f;
    const float ACC_Y = 0.1f;
    const float SCREEN_OFF = 16;
    const float JUMP_SPEED = -2.75f;

    // Check double jump timer
    if (pl->jumpTimer > 0.0f) {

        pl->jumpTimer -= 1.0f * tm;
        pl->speed.y = JUMP_SPEED * pl->jumpSpeedMul;
        pl->target.y = pl->speed.y;
    }

    // Store old position
    pl->oldPos = pl->pos;

    // Update axes
    update_axis(&pl->pos.x, &pl->speed.x, pl->target.x, ACC_X, tm);
    update_axis(&pl->pos.y, &pl->speed.y, pl->target.y, ACC_Y, tm);

    // Check screen boundaries
    if (pl->speed.x < 0 && pl->pos.x < SCREEN_OFF) {

        pl->pos.x = SCREEN_OFF;
        pl->speed.x = 0.0f;
    }
    else if(pl->speed.x > 0 && pl->pos.x > 256-SCREEN_OFF) {

        pl->pos.x = 256-SCREEN_OFF;
        pl->speed.x = 0.0f;
    }
}


// Animate
static void pl_animate(Player* pl, float tm) {

    const float EPS = 0.5f;
    const float FLIP_SPEED = 4.0f; 
    const float FLAP_SPEED = 4.0f;

    int frame = 0;
    int row = 0;

    bool drawGun = pl->shootWait > 0.0f;

    // Double jump
    if (pl->speed.y < 0.0f && !pl->doubleJump) {

        spr_animate(&pl->spr, 2, 0, 3, FLIP_SPEED, tm);
    }
    // Flapping
    else if (pl->flapping) {

        row = drawGun ? 4 : 3;
        spr_animate(&pl->spr, row, 0, 3, FLAP_SPEED, tm);
    }
    else {

        if (fabsf(pl->speed.y) < EPS)
            frame = 1;
        else if(pl->speed.y < 0.0f)
            frame = 2;

        pl->spr.frame = frame;
        pl->spr.row = 0;

        if (drawGun) {

            ++ pl->spr.row;
        }
    }
}


// Update dust
static void pl_update_dust(Player* pl, float tm) {

    const float DUST_SPEED = 1.25f;

    int i;

    if (!pl->dying && pl->respawnTimer <= 0.0f &&
        (pl->dustTimer += 1.0f * tm) >= DUST_WAIT) {

        pl->dustTimer -= DUST_WAIT;

        // Find the first dust that does not exist
        Dust* d = NULL;
        for (i = 0; i < DUST_COUNT; ++ i) {

            if (pl->dust[i].exist == false) {

                d = &pl->dust[i];
                break;
            }
        }
        if (d == NULL) return;

        dust_activate(d, vec2(pl->pos.x, pl->pos.y-24), DUST_SPEED,
            pl->spr.frame*pl->spr.width, pl->spr.row*pl->spr.height,
            pl->spr.width, pl->spr.height);
    }

    // Update dust
    for (i = 0; i < DUST_COUNT; ++ i) {

        dust_update(&pl->dust[i], tm);
    }
}


// Update bullet-specific behavior
static void pl_update_bullets(Player* pl, EventManager* evMan, float tm) {

    const float BULLET_RADIUS = 12;
    const float BIG_RADIUS = 24;
    const float LOAD_INITIAL = -60.0f;
    const float LOAD_BASE_WAIT = 30.0f;
    const float SPEED_MOD = 0.25f;
    const float SPEED_NORMAL = 5.0f;
    const float SPEED_BIG = 3.0f;
    const float SHOOT_ANIM_TIME = 60.0f;

    const float BULLET_X_OFF = 16;
    const float BULLET_Y_OFF = -21;

    const float REDUCE_POWER_NORMAL = 0.1f;
    const float REDUCE_POWER_BIG = 0.4f;

    int i;
    int s = pad_get_button_state(evMan->vpad, "fire2");

    // Create a bullet
    Bullet* b = NULL;
    bool makeBig = 
        pl->loading && 
        (s == StateReleased || s == StateUp) && 
        pl->loadTimer >= 0.0f;
    if (pl->stats->gunPower > 0.0f &&
        !pl->dying && pl->respawnTimer <= 0.0f && 
        pl->blastTime <= 0.0f && 
        (s == StatePressed || makeBig)) {

        for (i = 0; i < BULLET_COUNT; ++ i) {

            if (pl->bullets[i].exist == false) {

                b = &pl->bullets[i];
                break;
            }
        }

        if (b != NULL) {

            bullet_activate(b, 
                vec2(pl->pos.x+BULLET_X_OFF, pl->pos.y+BULLET_Y_OFF),
               
                vec2(pl->speed.x*SPEED_MOD + (makeBig ? SPEED_BIG : SPEED_NORMAL),
                     pl->speed.y*SPEED_MOD), 
                makeBig ? BIG_RADIUS : BULLET_RADIUS);

            pl->loading = (makeBig ? false : true);
            pl->loadTimer = LOAD_INITIAL;

            pl->shootWait = SHOOT_ANIM_TIME;
            pl->blastTime = BLAST_TIME;

            // Reduce power
            stats_modify_gun_power(pl->stats,
                makeBig ? -REDUCE_POWER_BIG : -REDUCE_POWER_NORMAL);
        }
    }

    // Update blast timer
    if (pl->blastTime > 0.0f) {

        pl->blastTime -= 1.0f * tm;
    }

    // Update load timer
    if (pl->loading) {

        pl->loadTimer += 1.0f * tm;
        if (pl->loadTimer > 0.0f) {

            pl->loadTimer = fmodf(pl->loadTimer, LOAD_BASE_WAIT);
        }
    
        if (pl->blastTime <= 0.0f &&
            (s == StateReleased || s == StateUp)) {

            pl->loading = false;
            pl->loadTimer = LOAD_INITIAL;
        }
    }

    // Update shoot timer
    if (pl->shootWait > 0.0f) {

        pl->shootWait -= 1.0f * tm;
    }

    // Update bullets
    for (i = 0; i < BULLET_COUNT; ++ i) {

        bullet_update(&pl->bullets[i], tm);
    }
}


// Respawn
static void pl_respawn(Player* pl) {

    const float INVINCIBILITY_TIME = 120.0f;

    pl->respawnTimer = RESPAWN_TIME;
    pl->spr.frame = 1;
    pl->spr.row = 0;

    // Reset
    pl->dying = false;
    pl->speed = vec2(0, 0);
    pl->target = vec2(0, 0);
    pl->loading = false;
    pl->quickFall = false;
    pl->doubleJump = true;
    pl->dustTimer = 0;
    pl->invincibilityTimer = INVINCIBILITY_TIME;
    pl->djumpReleased = false;
    pl->jumpTimer = 0.0f;
    pl->selfDestructTimer = 0.0f;

    pl->pos = pl->startPos;
}


// Die
static void pl_die(Player* pl, float globalSpeed, float tm) {

    const float DEATH_ANIM_SPEED[] = {
        7.0f, 4.0f
    };
    const float END_FRAME[] = {
        3, 5
    };

    int i;
    int type = pl->spr.row -5;

    // Explosion, do not animate a thing
    bool dead = exp_dead(&pl->exp);
    if (pl->exp.exist || dead) {

        if (dead) {
            
            pl_respawn(pl);
        }

        return;
    }

    if (type == 0)
        pl->pos.y = 192-GROUND_COLLISION_HEIGHT;
    pl->pos.x -= globalSpeed * tm;

    if (pl->spr.frame < END_FRAME[type] ) {

        spr_animate(&pl->spr, pl->spr.row, 0, END_FRAME[type], 
            DEATH_ANIM_SPEED[type], tm);

        if(pl->spr.frame == END_FRAME[type]) {

            if (type == 0) {
                // Create a body
                for (i = 0; i < BODY_COUNT; ++ i) {

                    if (pl->bodies[i].exist == false) {

                        body_activate(&pl->bodies[i], pl->pos, pl->spr);
                        break;
                    }
                }
            }

            pl_respawn(pl);
            
        }
    }
}


// Create a player
Player create_player(int x, int y, Stats* stats) {

    Player pl;

    // Set position & speeds
    pl.pos = vec2((float)x, (float)y);
    pl.oldPos = pl.pos;
    pl.startPos = pl.pos;
    pl.speed = vec2(0, 0);
    pl.target = vec2(0, 0);

    // Set other initial values
    pl.jumpTimer = 0.0f;
    pl.doubleJump = true;
    pl.flapping = false;
    pl.djumpReleased = false;
    pl.extendJump = false;
    pl.quickFall = false;
    pl.loading = false;
    pl.loadTimer = 0.0f;
    pl.blastTime = 0.0f;
    pl.dustTimer = 0.0f;
    pl.jumpSpeedMul = 1.0f;
    pl.shootWait = 0.0f;
    pl.dying = false;
    pl.stats = stats;
    
    // Create sprite
    pl.spr = create_sprite(48, 48);

    // Respawn
    pl_respawn(&pl);

    // No need to be invincible now
    pl.invincibilityTimer = 0.0f;

    // Create dust
    int i;
    for (i = 0; i < DUST_COUNT; ++ i) {

        pl.dust[i] = create_dust();
    }

    // Create bullets
    for (i = 0; i < BULLET_COUNT; ++ i) {

        pl.bullets[i] = create_bullet();
    }

    // Create bodies
    for (i = 0; i < BODY_COUNT; ++ i) {

        pl.bodies[i] = create_body();
    }

    // Create explosion
    pl.exp = create_explosion();

    return pl;
}


// Update player
void pl_update(Player* pl, EventManager* evMan, float globalSpeed, float tm,
    void* coins, int coinLen) {

    const float ARROW_WAVE_SPEED = 0.15f;

    // Update explosion
    exp_update(&pl->exp, tm);

    // Update dust
    pl_update_dust(pl, tm);
    // Update bullets
    pl_update_bullets(pl, evMan, tm);

    // Update bodies
    int i;
    for (i = 0; i < BODY_COUNT; ++ i) {

        body_update(&pl->bodies[i], globalSpeed, tm);
    }

    // Update invincibility timer
    if (pl->invincibilityTimer > 0.0f) {

        pl->invincibilityTimer -= 1.0f * tm;
    }

    // Respawn
    if (pl->respawnTimer > 0.0f) {

        pl->respawnTimer -= 1.0f * tm;
        return;
    }

    // Die
    if (pl->dying) {

        pl_die(pl, globalSpeed, tm);
        return;
    }

    // Do stuff
    pl_control(pl, evMan, tm, 
        (Coin*)coins, coinLen, globalSpeed);
    // Might happen if self-destructing
    if (pl->dying) return;

    pl_move(pl, evMan, tm);
    pl_animate(pl, tm);

    // Update arrow wave
    pl->arrowWave += ARROW_WAVE_SPEED * tm;
    pl->arrowWave = fmodf(pl->arrowWave, M_PI*2);

    // Die
    if (pl->pos.y > 192-GROUND_COLLISION_HEIGHT) {

        pl_kill(pl, 0);
    }
}


// Shake
void pl_shake(Player* pl, Graphics* g) {

    exp_shake(&pl->exp, g);
}


// Draw the player shadow
void pl_draw_shadow(Player* pl, Graphics* g) {

    const float SHADOW_SCALE_COMPARE = 176+128;
    const float COMPARE_DELTA = 128;
    const int SHADOW_MOVE_Y = 3;
    const int SHADOW_DARK_VALUE = 5;

    int px = (int)roundf(pl->pos.x);
    int py = (int)roundf(pl->pos.y);

    // Draw shadow
    int scale = (int)((pl->pos.y+COMPARE_DELTA)/
        SHADOW_SCALE_COMPARE *48.0f);

    g_set_pixel_function(g, 
        PixelFunctionDarken, 
        SHADOW_DARK_VALUE, 0);
    g_draw_scaled_bitmap_region(g, bmpBunny,
        144, 0, 48, 48, 
        px-scale/2, 
        192-GROUND_COLLISION_HEIGHT+SHADOW_MOVE_Y - scale, 
        scale, scale, false);
    g_set_pixel_function(g, PixelFunctionDefault, 0, 0);
}


// Draw entrance portal
void pl_draw_entrance_portal(Player* pl, Graphics* g) {

    const float RADIUS_MAX = 48.0f;

    if (pl->respawnTimer <= 0.0f) return;

    float t = pl->respawnTimer / RESPAWN_TIME;
    float r = RADIUS_MAX * (1.0f-t);

    float angle = t * M_PI;

    int cx = (int)roundf(pl->pos.x);
    int cy = (int)roundf(pl->pos.y) - pl->spr.height/2;

    float step = M_PI*2.0f / 3.0f;
    int dvalue = (int) floorf(t * 14);

    g_toggle_texturing(g, NULL);
    g_set_pixel_function(g, PixelFunctionDarken, dvalue, 0);
    g_draw_triangle(
        g, 
        cx + (int)(cosf(angle)*r), cy + (int)(sinf(angle)*r),
        cx + (int)(cosf(angle+step)*r), cy + (int)(sinf(angle+step)*r),
        cx + (int)(cosf(angle+step*2)*r), cy + (int)(sinf(angle+step*2)*r),
        255
    );
    g_set_pixel_function(g, PixelFunctionDefault, 0, 0);
}


// Draw player
void pl_draw(Player* pl, Graphics* g) {

    const int ARROW_Y = 4;
    const int ARROW_AMPLITUDE = 4;

    int i;
    int frame;

    int px = (int)roundf(pl->pos.x);
    int py = (int)roundf(pl->pos.y);


    // Draw dust
    for (i = 0; i < DUST_COUNT; ++ i) {

        dust_draw(&pl->dust[i], g, bmpBunny, 255);
    }

    // Draw bodies
    for (i = 0; i < BODY_COUNT; ++ i) {

        body_draw(&pl->bodies[i], g, bmpBunny);
    }

    // If exploding, stop here
    if (pl->exp.exist)
        return;

    // Draw respawning
    int dvalue = 0;
    int sx, sy;
    float t;
    if (pl->respawnTimer > 0.0f) {

        t = 1.0f - pl->respawnTimer / RESPAWN_TIME;

        // Set scale
        sx = (int)roundf(pl->spr.width * t);
        sy = (int)roundf(pl->spr.height * t);

        // Draw sprite
        spr_draw_scaled(&pl->spr, g, bmpBunny, 
            px - sx/2, 
            py-24 - sy/2,
            sx, sy,
            false);

        return;
    }
    
    // Draw gun behind the bunny while 
    // double jumping
    if (pl->doubleJump == false && pl->speed.y < 0.0f
        && pl->shootWait > 0.0f ) {

        spr_draw_frame(&pl->spr, g, 
            bmpBunny, px-24, py-48, 3, 1, false);
    }

    // Draw sprite
    if (pl->loading && pl->loadTimer > 0.0f && 
            (int)floorf(pl->loadTimer/4.0f) % 2 == 0) {

        g_set_pixel_function(g, PixelFunctionInverse, 0, 0);
    }
    else if(pl->invincibilityTimer > 0.0f &&
            (int)floorf(pl->invincibilityTimer/4) % 2 == 1) {

         g_set_pixel_function(g, PixelFunctionSkipSimple, 0, 0);
    }

    spr_draw(&pl->spr, g, bmpBunny, 
        px-24, 
        py-48, false);
    g_set_pixel_function(g, PixelFunctionDefault, 0, 0);


    // Draw bullets
    for (i = 0; i < BULLET_COUNT; ++ i) {

        bullet_draw(&pl->bullets[i], g);
    }

    // Draw blast
    if (pl->blastTime > 0.0f) {

        frame = 2 - (int) floorf(
            fabsf(pl->blastTime - BLAST_TIME/2.0f) /
            (BLAST_TIME/2.0f)*3.0f);

        g_draw_bitmap_region(g, bmpBlast, frame*32, 0, 32, 32,
            px + 20, py - 40, false);
    }

    // Draw arrow
    if (pl->pos.y < 0.0f) {

        g_draw_bitmap(g, bmpArrow, 
            px-24, 
            ARROW_Y + (int)(sinf(pl->arrowWave)*ARROW_AMPLITUDE), 
            false);
    }
}


// Draw player explosion
void pl_draw_explosion(Player* pl, Graphics* g) {

    exp_draw(&pl->exp, g);
}


// Jump collision
bool pl_jump_collision(Player* pl, float x, float y, float w, float power) {

    const float JUMP_SPEED_MUL_FACTOR = 0.5f;

    float mul = pl->speed.y / GRAVITY_TARGET;
    float s = fabsf(pl->speed.y);

    if (pl->speed.y > 0.0f &&
        pl->pos.x > x && pl->pos.x < x+w &&
        pl->oldPos.y-s < y+pl->speed.y &&
        pl->pos.y+s > y-pl->speed.y) {

        pl->pos.y = y;

        pl->doubleJump = true;
        pl->djumpReleased = false;
        pl->extendJump = false;

        pl->jumpTimer = power * mul;
        pl->quickFall = false;

        pl->jumpSpeedMul = 1.0f + (mul-1.0f)*JUMP_SPEED_MUL_FACTOR;

        return true;
    }

    return false;
}


// Kill a player
void pl_kill(Player* pl, int type) {

    if (pl->dying || pl->respawnTimer > 0.0f) return;

    pl->dying = true;
    pl->spr.row = 5 + type;
    pl->spr.frame = 0;
    pl->spr.count = 0;
    pl->selfDestructTimer = 0.0f;
    stats_reset_power(pl->stats);

    if (pl->stats->lives > 0)
        -- pl->stats->lives;
}
