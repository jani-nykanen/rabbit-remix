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


// Float
static void coin_float(Coin* c, float globalSpeed, float tm) {

    const float AMPLITUDE = 4.0f;
    const float FLOAT_SPEED = 0.1f;

    // Update floating
    c->floatTimer += FLOAT_SPEED * tm;
    c->floatTimer = fmodf(c->floatTimer, 2 * M_PI);
    c->pos.y = c->startPos.y + sinf(c->floatTimer) * AMPLITUDE;

    // Move
    c->pos.x -= globalSpeed * tm;
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
void coin_activate(Coin* c, Vector2 pos, Vector2 speed, 
    int type, bool floating) {

    const float WAIT_TIME = 20.0f;

    c->pos = pos;
    c->startPos = pos;
    c->speed = speed;

    c->exist = true;
    c->dying = false;
    c->wait = floating ? 0.0f : WAIT_TIME;

    if (type == 0) {

        c->spr = create_sprite(20, 20);
        
    }
    else {

        c->spr = create_sprite(24, 24);
    }
    c->type = type;
    c->floating = floating;
}


// Update a coin
void coin_update(Coin* c, float globalSpeed, float tm) {

    const float SPEED_LIMIT = 1.25f;
    const float COLLISION_MUL = 0.90f;
    const float GRAVITY_DELTA = 0.05f;
    const float GRAVITY_MAX = 4.0f;
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

    // Kill if outside the screen
    if (c->pos.x + c->spr.width/2 < 0) {
        
        c->exist = false;
        return;
    }

    // Animate
    spr_animate(&c->spr, c->type, 
        0, 3, ANIM_SPEED, tm);

    // Float
    if (c->floating) {

        coin_float(c, globalSpeed, tm);
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
}


// Coin-player collision
void coin_player_collision(Coin* c, Player* pl) {

    const float GUN_POWER_BONUS = 0.5f;

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

    int hitW = c->spr.width/2;
    int hitH = c->spr.height/2;

    int i;
    for (i = -1; i <= 1; ++ i) {

        if (px + pw/2 > cx+i*256-hitW/2 &&
            px - pw/2 < cx+i*256+hitW/2 &&
            py > cy - hitH/2 &&
            py - ph < cy + hitH/2) {

            c->dying = true;
            c->deathTimer = DEATH_TIME;

            // switch? Why bother
            if (c->type == 0) {
                    
                // Add a coin to stats  
                stats_add_coins(pl->stats, 1);
            }
            else if(c->type == 1) {

                // Add some energy
                stats_modify_gun_power(pl->stats, GUN_POWER_BONUS);
            }
            else {

                // Add a life
                stats_add_life(pl->stats);
            }

            break;
        }
    }
}


// Draw a coin
void coin_draw(Coin* c, Graphics* g) {

    const float DEATH_SCALE = 1.5f;

    if (!c->exist) return;

    // int i;

    int sx, sy;
    float t;
    int skip;

    //int begin = !c->floating ? -1 : 0;
    //int end = !c->floating ? 1 : 0;

    //for (i = begin; i <= end; ++ i) {

        if (c->dying) {

            t = c->deathTimer / DEATH_TIME;
            sx = (int)((1.0f + (1.0f-t)*(DEATH_SCALE-1.0f)) * c->spr.width);
            sy = (int)((1.0f + (1.0f-t)*(DEATH_SCALE-1.0f)) * c->spr.height);

            skip = 1 + (int) floorf( t * c->spr.height/2);
            
            g_set_pixel_function(g, PixelFunctionSkip, skip, 0);

            // Draw scaled sprite
            spr_draw_scaled(&c->spr, g, bmpCoin, 
                (int)roundf(c->pos.x)-sx/2, // i*g->csize.x,
                (int)roundf(c->pos.y)-c->spr.height/2 - sy/2,
                sx, sy,
                false);

            g_set_pixel_function(g, PixelFunctionDefault, 0, 0);
        }
        else {

            // Draw sprite
            spr_draw(&c->spr, g, bmpCoin, 
                (int)roundf(c->pos.x)-c->spr.width/2, // i*g->csize.x,
                (int)roundf(c->pos.y)-c->spr.height,
                false);
        }
    //}
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
