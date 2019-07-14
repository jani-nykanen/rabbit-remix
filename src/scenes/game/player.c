#include "player.h"

#include <engine/mathext.h>

#include "stage.h"

// Shared pointers to bitmaps
static Bitmap* bmpBunny;
static Bitmap* bmpBlast;

// Constants
static const float JUMP_REACT_MIN_TIME = 1.0f;
static const float JUMP_EXTEND_TIME = 10.0f;
static const float DUST_WAIT = 8.0f;
static const float BLAST_TIME = 10.0f;
static const float GRAVITY_TARGET = 3.0f;


// Init global data
void init_global_player(AssetManager* a) {

    bmpBunny = (Bitmap*)assets_get(a, "bunny");
    bmpBlast = (Bitmap*)assets_get(a, "blast");
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


// Control
static void pl_control(Player* pl, EventManager* evMan, float tm) {

    const float MOVE_TARGET = 1.5f;
    const float FLAP_SPEED = 0.5f;
    const float DJUMP_TIME = 7.0f;

    const float QUICK_FALL_EPS = 0.5f;
    const float QUICK_FALL_MUL = 1.5f;

    // Set target speed
    pl->target.x = evMan->vpad->stick.x * MOVE_TARGET;
    pl->target.y = GRAVITY_TARGET;
    
    // Quick fall
    if (!pl->quickFall &&
        evMan->vpad->stick.y > QUICK_FALL_EPS &&
        evMan->vpad->delta.y > 0.0f) {

        pl->quickFall = true;
        pl->speed.y = pl->target.y * QUICK_FALL_MUL;
    }
    if (evMan->vpad->stick.y < 0.0f) {

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

    if ( (pl->dustTimer += 1.0f * tm) >= DUST_WAIT) {

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

    int i;
    int s = pad_get_button_state(evMan->vpad, "fire2");

    // Create a bullet
    Bullet* b = NULL;
    bool makeBig = 
        pl->loading && 
        (s == StateReleased || s == StateUp) && 
        pl->loadTimer >= 0.0f;
    if (pl->blastTime <= 0.0f && 
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


// Create a player
Player create_player(int x, int y) {

    Player pl;

    // Set position & speeds
    pl.pos = vec2((float)x, (float)y);
    pl.oldPos = pl.pos;
    pl.speed = vec2(0, 0);
    pl.target = vec2(0, 0);

    // Set other initial values
    pl.jumpTimer = 0.0f;
    pl.doubleJump = false;
    pl.flapping = false;
    pl.djumpReleased = false;
    pl.extendJump = false;
    pl.quickFall = false;
    pl.loading = false;
    pl.loadTimer = 0.0f;
    pl.blastTime = 0.0f;
    pl.dustTimer = 0.0f;
    pl.jumpSpeedMul = 1.0f;

    // Create sprite
    pl.spr = create_sprite(48, 48);

    // Create dust
    int i;
    for(i = 0; i < DUST_COUNT; ++ i) {

        pl.dust[i] = create_dust();
    }

    // Create bullets
    for(i = 0; i < BULLET_COUNT; ++ i) {

        pl.bullets[i] = create_bullet();
    }

    return pl;
}


// Update player
void pl_update(Player* pl, EventManager* evMan, float tm) {


    // Do stuff
    pl_control(pl, evMan, tm);
    pl_move(pl, evMan, tm);
    pl_update_bullets(pl, evMan, tm);
    pl_animate(pl, tm);
    pl_update_dust(pl, tm);

    // TEMP
    // Floor collision
    pl_jump_collision(pl, 0, 192-GROUND_COLLISION_HEIGHT, 256, 10.0f);
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


// Draw player
void pl_draw(Player* pl, Graphics* g) {

    int i;
    int frame;

    int px = (int)roundf(pl->pos.x);
    int py = (int)roundf(pl->pos.y);

    // Draw dust
    for (i = 0; i < DUST_COUNT; ++ i) {

        dust_draw(&pl->dust[i], g, bmpBunny, 255);
    }

    // Draw sprite
    if (pl->loading && pl->loadTimer > 0.0f && 
        (int)floorf(pl->loadTimer/4.0f) % 2 == 0) {

        g_set_pixel_function(g, PixelFunctionInverse, 0, 0);
    }
    spr_draw(&pl->spr, g, bmpBunny, 
        px-24, 
        py-48, false);
    g_set_pixel_function(g, PixelFunctionDefault, 0, 0);

    // Draw bullets
    for (i = 0; i < BULLET_COUNT; ++ i) {

        bullet_draw(&pl->bullets[i], g);
    }

    // Draw blast (if not double jumping)
    if (pl->blastTime > 0.0f && 
        !(pl->speed.y < 0.0f && !pl->doubleJump)) {

        frame = 2 - (int) floorf(fabsf(pl->blastTime - BLAST_TIME/2.0f)/(BLAST_TIME/2.0f)*3.0f);

        g_draw_bitmap_region(g, bmpBlast, frame*32, 0, 32, 32,
            px + 20, py - 40, false);
    }
}


// Jump collision
bool pl_jump_collision(Player* pl, float x, float y, float w, float power) {

    const float JUMP_SPEED_MUL_FACTOR = 0.5f;

    float mul = pl->speed.y / GRAVITY_TARGET;

    if (pl->speed.y > 0.0f &&
        pl->pos.x > x && pl->pos.x < x+w &&
        pl->oldPos.y < y+pl->speed.y &&
        pl->pos.y > y-pl->speed.y) {

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
