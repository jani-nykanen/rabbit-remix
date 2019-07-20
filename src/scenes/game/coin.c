#include "coin.h"

#include "stage.h"

#include <math.h>

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

    c.spr = create_sprite(16, 16);

    return c;
}

// Activate a coin
void coin_activate(Coin* c, Vector2 pos, Vector2 speed) {

    c->pos = pos;
    c->speed = speed;

    c->exist = true;
}


// Update a coin
void coin_update(Coin* c, float globalSpeed, float tm) {

    const float SPEED_LIMIT = 0.5f;
    const float COLLISION_MUL = 0.90f;
    const float GRAVITY_DELTA = 0.15f;
    const float GRAVITY_MAX = 3.0f;
    const float SPEED_DELTA = 0.01f;
    const float ANIM_SPEED = 6.0f;

    if (!c->exist) return;

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

    if (!c->exist || pl->dying || pl->respawnTimer > 0.0f) 
        return;

    // REMEMBER: Three times collision, for looping
    // positions also
}


// Draw a coin
void coin_draw(Coin* c, Graphics* g) {

    if (!c->exist) return;

    int i;

    for (i = -1; i <= 1; ++ i) {

        // Draw sprite
        spr_draw(&c->spr, g, bmpCoin, 
            (int)roundf(c->pos.x)-8 + i*g->csize.x,
            (int)roundf(c->pos.y)-16,
            false);
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
