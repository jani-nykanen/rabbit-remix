#include "player.h"

// Shared pointers to bitmaps
static Bitmap* bmpBunny;


// Init global data
void init_global_player(AssetManager* a) {

    bmpBunny = (Bitmap*)assets_get(a, "bunny");
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

    const float GRAVITY_TARGET = 2.5f;

    pl->target.x = 0.0f;
    pl->target.y = GRAVITY_TARGET;
}


// Move
static void pl_move(Player* pl, EventManager* evMan, float tm) {

    const float ACC_X = 0.1f;
    const float ACC_Y = 0.1f;

    // Store old position
    pl->oldPos = pl->pos;

    // Update axes
    update_axis(&pl->pos.x, &pl->speed.x, pl->target.x, ACC_X, tm);
    update_axis(&pl->pos.y, &pl->speed.y, pl->target.y, ACC_Y, tm);
}


// Animate
static void pl_animate(Player* pl, float tm) {

    const float EPS = 0.5f;

    int frame = 0;
    if (fabsf(pl->speed.y) < EPS)
        frame = 1;
    else if(pl->speed.y < 0.0f)
        frame = 2;

    pl->spr.frame = frame;
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

    // Create sprite
    pl.spr = create_sprite(48, 48);

    return pl;
}


// Update player
void pl_update(Player* pl, EventManager* evMan, float tm) {

    // Do stuff
    pl_control(pl, evMan, tm);
    pl_move(pl, evMan, tm);
    pl_animate(pl, tm);

    // TEMP
    // Floor collision
    pl_jump_collision(pl, 0, 192-16, 256);
}


// Draw player
void pl_draw(Player* pl, Graphics* g) {

    const float SHADOW_SCALE_COMPARE = 192-16;

    int px = (int)roundf(pl->pos.x);
    int py = (int)roundf(pl->pos.y);

    // Draw shadow
    int scale = (int)(pl->pos.y/SHADOW_SCALE_COMPARE *48.0f);

    // TEMP:
    g->dvalue = 9;
    g_draw_scaled_bitmap_region(g, bmpBunny,
        144, 0, 48, 48, 
        px-scale/2, 192-16 - scale, 
        scale, scale, false);

    // Draw sprite
    spr_draw(&pl->spr, g, bmpBunny, 
        px-24, 
        py-48, false);
}


// Jump collision
void pl_jump_collision(Player* pl, float x, float y, float w) {

    const float HIT_JUMP_HEIGHT = -4.0f;

    if (pl->speed.y > 0.0f &&
        pl->pos.x > x && pl->pos.x < x+w &&
        pl->oldPos.y < y+pl->speed.y &&
        pl->pos.y > y-pl->speed.y) {

        pl->pos.y = y;
        pl->speed.y = HIT_JUMP_HEIGHT;
    }
}
