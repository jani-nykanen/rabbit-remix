#include "coin.h"

#include "stage.h"

#include <math.h>

// Constants
static const float DEATH_TIME = 15.0f;

// Bitmaps
static Bitmap* bmpCoin;


// Init global
void init_global_coins(AssetManager* a) {

    // Get bitmaps
    bmpCoin = (Bitmap*)assets_get(a, "coin");
}


// Create a coin
Coin create_coin() {

    Coin c;
    c.exist = false;
    c.dying = false;

    c.spr = create_sprite(20, 20);

    return c;
}

// Activate a coin
void coin_activate(Coin* c, Vector2 pos, Vector2 speed) {

    const float WAIT_TIME = 20.0f;

    c->pos = pos;
    c->speed = speed;

    c->exist = true;
    c->dying = false;

    c->wait = WAIT_TIME;
}


// Update a coin
void coin_update(Coin* c, float globalSpeed, float tm) {

    const float SPEED_LIMIT = 1.25f;
    const float COLLISION_MUL = 0.90f;
    const float GRAVITY_DELTA = 0.05f;
    const float GRAVITY_MAX = 2.0f;
    const float SPEED_DELTA = 0.005f;
    const float ANIM_SPEED = 6.0f;

    if (!c->exist) return;

    // Update death
    if (c->dying) {

        if ( (c->deathTimer -= 1.0f * tm) <= 0.0f) {

            c->exist = false;
            return;
        }

        c->pos.x -= globalSpeed * tm;

        return;
    }

    // Update wait
    if (c->wait > 0.0f)
        c->wait -= 1.0f * tm;

    // Update horizontal speed
    if (c->speed.x > 0.0f) {

        c->speed.x -= SPEED_DELTA * tm;
        if (c->speed.x < 0.0f)
            c->speed.x = 0.0f;
    }
    else if (c->speed.x < 0.0f) {

        c->speed.x += SPEED_DELTA * tm;
        if (c->speed.x > 0.0f)
            c->speed.x = 0.0f;
    }

    // Update gravity
    if (c->speed.y < GRAVITY_MAX) {

        c->speed.y += GRAVITY_DELTA * tm;
        if (c->speed.y > GRAVITY_MAX)
            c->speed.y = GRAVITY_MAX;
    }

    // Move
    c->pos.x += (c->speed.x - globalSpeed) * tm;
    c->pos.y += (c->speed.y) * tm;

    // Ground collision
    if (c->speed.y > 0.0f &&
        c->pos.y > 192-GROUND_COLLISION_HEIGHT) {

        c->pos.y = 192-GROUND_COLLISION_HEIGHT;
        if (c->speed.y < SPEED_LIMIT) {

            c->exist = false;
            return;
        }

        c->speed.y *= -COLLISION_MUL;
    }

    // Loop
    if (c->pos.x < 0)
        c->pos.x += 256;
    else if(c->pos.x > 256)
        c->pos.x -= 256;

    // Animate
    spr_animate(&c->spr, 0, 0, 3, ANIM_SPEED, tm);
}


// Coin-player collision
void coin_player_collision(Coin* c, Player* pl) {

    const float HIT_W = 12.0f;
    const float HIT_H = 12.0f;

    if (c->wait > 0 || !c->exist || c->dying || 
        pl->dying || pl->respawnTimer > 0.0f) 
        return;

    // REMEMBER: Three times collision, for looping
    // positions also

    float px = pl->pos.x;
    float py = pl->pos.y;

    float pw = pl->spr.width;
    float ph = pl->spr.height;

    float cx = c->pos.x;
    float cy = c->pos.y - c->spr.height/2;

    int i;
    for (i = -1; i <= 1; ++ i) {

        if (px + pw/2 > cx+i*256-HIT_W/2 &&
            px - pw/2 < cx+i*256+HIT_W/2 &&
            py > cy - HIT_H/2 &&
            py - ph < cy + HIT_H/2) {

            c->dying = true;
            c->deathTimer = DEATH_TIME;

            // Add a coin to stats
            stats_add_coins(pl->stats, 1);

            break;
        }
    }
}


// Draw a coin
void coin_draw(Coin* c, Graphics* g) {

    const float DEATH_SCALE = 1.5f;

    if (!c->exist) return;

    int i;

    int sx, sy;
    float t;
    int skip;

    for (i = -1; i <= 1; ++ i) {

        if (c->dying) {

            t = c->deathTimer / DEATH_TIME;
            sx = (int)((1.0f + (1.0f-t)*(DEATH_SCALE-1.0f)) * c->spr.width);
            sy = (int)((1.0f + (1.0f-t)*(DEATH_SCALE-1.0f)) * c->spr.height);

            skip = 1 + (int) floorf( t * 10);
            
            g_set_pixel_function(g, PixelFunctionSkip, skip, 0);

            // Draw scaled sprite
            spr_draw_scaled(&c->spr, g, bmpCoin, 
                (int)roundf(c->pos.x)-sx/2 + i*g->csize.x,
                (int)roundf(c->pos.y)-10 - sy/2,
                sx, sy,
                false);

            g_set_pixel_function(g, PixelFunctionDefault, 0, 0);
        }
        else {

            // Draw sprite
            spr_draw(&c->spr, g, bmpCoin, 
                (int)roundf(c->pos.x)-10 + i*g->csize.x,
                (int)roundf(c->pos.y)-20,
                false);
        }
    }
}   


// Get the next available coin in the array
Coin* coin_get_next(Coin* coins, int len) {

    int i;
    for (i = 0; i < len; ++ i) {

        if (coins[i].exist == false) {

            return &coins[i];
        }
    }
    return NULL;
}
