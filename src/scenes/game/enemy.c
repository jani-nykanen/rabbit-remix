#include "enemy.h"

#include <engine/mathext.h>

#include <math.h>

// Constants
static const float DEATH_TIME = 20.0f;

// Global bitmaps
static Bitmap* bmpEnemy;


// Initialize global enemy content
void init_global_enemies(AssetManager* a) {

    // Get bitmaps
    bmpEnemy = (Bitmap*)assets_get(a, "enemy");
}


// Create coins
static void enemy_create_coins(Enemy* e, 
    Coin* coins, int len, int min, int max,
    bool stomp) {

    const float SPEED_VARY_X_RIGHT = 2.5f;
    const float SPEED_VARY_X_LEFT = -1.5f;
    const float SPEED_VARY_Y_UP = -3.0f;
    const float SPEED_VARY_Y_DOWN = 1.0f;
    const float Y_OFF = 0.0f;
    const int GEM_PROB = 25;
    const float GEM_SPEED_Y = 0.0f;

    int i;
    int loop = (rand() % (max-min)) + min;

    Vector2 speed;
    Vector2 pos = e->pos;
    pos.y += Y_OFF;
    Coin* c;

    for (i = 0 ; i < loop; ++ i) {

        speed.x = (float)(rand() % 100)/100.0f * 
            (SPEED_VARY_X_RIGHT - SPEED_VARY_X_LEFT) + SPEED_VARY_X_LEFT ;
        speed.y = (float)(rand() % 100)/100.0f * 
            (SPEED_VARY_Y_DOWN - SPEED_VARY_Y_UP) + SPEED_VARY_Y_UP ;

        if (stomp) {

            speed.y = fabsf(speed.y);
        }

        c = coin_get_next(coins, len);
        if (c == NULL) break;

        // Activate
        coin_activate(c, pos, speed, 0, false);
    }

    // Might create a gem, if not stomped
    if (!stomp && rand() % 100 < GEM_PROB) {

        c = coin_get_next(coins, len);
        if (c == NULL) return;

        coin_activate(c, pos, 
            vec2(0.0f, GEM_SPEED_Y), 
            1, false);
    }
}



// Kill
static void enemy_kill(Enemy* e, Stats* s, 
    Coin* coins, int coinLen, 
    bool stomped,
    Message* msgs, int msgLen) {

    const float POWER_PLUS = 0.25f;
    const int COIN_MIN = 1;
    const int COIN_MAX = 3;
    const int SCORE = 250;

    e->deathTimer = DEATH_TIME;
    e->dying = true;

    // Add power to the star meter
    stats_modify_power(s, POWER_PLUS);
    // Create coins
    enemy_create_coins(e, coins, coinLen,
        COIN_MIN, COIN_MAX, stomped);

    // Create message & add points
    int score = SCORE;
    score += (SCORE/10) * s->coins;
    msg_create_score_message(msgs, msgLen, score, e->pos);
    stats_add_points(s, score);
}


//
// Enemy AI
//
// Type 0
static void enemy_update_type0(Enemy* e, 
    float globalSpeed, float tm) {

    const float WAVE_SPEED_X = 0.025f;
    const float WAVE_SPEED_Y = 0.05f;

    const float AMPLITUDE_X = 0.5f;
    const float AMPLITUDE_Y = 12.0f;

    // Waves
    if (e->hurtTimer <= 0.0f) {
            
        e->wave.x += WAVE_SPEED_X * tm;
        e->speed.x = 1.0f + sinf(e->wave.x) * AMPLITUDE_X;
    }

    e->wave.y += WAVE_SPEED_Y * tm;
    e->pos.y = e->startPos.y + sinf(e->wave.y) * AMPLITUDE_Y;    
}


// Type 1
static void enemy_update_type1(Enemy* e, 
    float globalSpeed, float tm) {

    const float STOP_X = 256;
    const float SPEED_DELTA = 0.05f;

    if (e->phase == 0) {

        e->speed.x = 1.0f;  
        if (e->pos.x < STOP_X) {

            e->pos.x = STOP_X;
            e->speed.x = 0.0f;
            ++ e->phase;
        }
    }
    else {

        e->speed.x += SPEED_DELTA * tm;
    }
}



// Update special behavior
static void enemy_update_special(Enemy* e, float globalSpeed, float tm) {

    
    switch (e->id)
    {
    case 0:
        enemy_update_type0(e, globalSpeed, tm);
        break;
    case 1:
        enemy_update_type1(e, globalSpeed, tm);
        break;
    
    default:
        break;
    }
}


// Draw dying
static void enemy_draw_dying(Enemy* e, Graphics* g) {

    const float SCALE_VALUE = 1.5f;

    float t = 1.0f - e->deathTimer / DEATH_TIME;
    int dvalue =  (int)floorf( (1.0f-t) * 14.0f);

    int x = (int) roundf(e->pos.x) - e->spr.width/2;
    int y = (int) roundf(e->pos.y) - e->spr.height/2;

    float s = SCALE_VALUE - 1.0f;
    int sx = (int)e->spr.width * (1.0f + t*s);
    int sy = (int)e->spr.height * (1.0f + t*s);

    x -= (sx - e->spr.width)/2;
    y -= (sy - e->spr.height)/2;

    g_set_pixel_function(g, PixelFunctionLighten, dvalue, 0);

    // Draw scaled sprite
    spr_draw_scaled(&e->spr, g, bmpEnemy,
        x, y, sx, sy, false);

    g_set_pixel_function(g, PixelFunctionDefault, 0, 0);
}


// Create an enemy
Enemy create_enemy(){

    Enemy e;
    e.exist = false;

    return e;
}


// Activate an enemy
void enemy_activate(Enemy* e, Vector2 pos, int id){
    
    const float RADIUS[] = {
        16.0f, 12.0f,
    };
    const int HIT_POINTS[] = {
        2, 1
    };

    // Set stuff
    e->spr = create_sprite(48, 48);
    e->pos = pos;
    e->startPos = pos;
    e->id = id;
    e->radius = RADIUS[id];
    e->health = HIT_POINTS[id];
    e->dying = false;
    e->wave = vec2(
        (float)(rand() % 100)/100.0f * M_PI * 2.0f,
        (float)(rand() % 100)/100.0f * M_PI * 2.0f
    );
    e->hurtTimer = 0.0f;
    e->phase = 0.0f;

    switch(e->id) {

    case 0:
        e->speed.x = 0.5f;
        break;

    default:    
        e->speed.x = 1.0f;
        break;
    }

    e->exist = true;
}


// Update an enemy
void enemy_update(Enemy* e, float globalSpeed, float tm){
    
    const float ANIM_SPEED = 6.0f;
    const float KNOCKBACK_SPEED = -0.5f;

    if (!e->exist) return;

    // Die
    if (e->dying) {

        if ((e->deathTimer -= 1.0f * tm) <= 0.0f) {

            e->exist = false;
            e->dying = false;
        }

        return;
    }

    // Update hurt timer
    if (e->hurtTimer > 0.0f) {

        e->hurtTimer -= 1.0f * tm;
    }

    // Update special
    enemy_update_special(e, globalSpeed, tm);

    // Animate
    spr_animate(&e->spr, e->id, 0, 3, ANIM_SPEED, tm);

    // Move
    float s = e->speed.x;
    if (e->hurtTimer > 0.0f) {

        s *= KNOCKBACK_SPEED;
    }

    e->pos.x -= s * globalSpeed * tm;

    // See if has "gone too far"
    if (e->pos.x + e->spr.width/2 < 0) {

        e->exist = false;
    }
}


// Bullet-enemy collision
void enemy_bullet_collision(
    Enemy* e, Bullet* b, Stats* s, 
    Coin* coins, int coinLen,
    Message* msgs, int msgLen){
        
    const float HURT_TIME = 30.0f;

    if (!e->exist || !b->exist || b->dying || e->dying) 
        return;

    // Check if inside the collision area
    if (hypotf(b->pos.x-e->pos.x, b->pos.y-e->pos.y) < e->radius+b->radius) {
         
        if (!b->isSpecial)
            bullet_kill(b);

        if (b->isSpecial || --e->health <= 0) {

            enemy_kill(e, s, coins, coinLen, false,
                msgs, msgLen);
        }
        else {

            e->hurtTimer = HURT_TIME;
            e->wave.x = fmodf(e->wave.x, M_PI);
        }
    }
}


// Player-enemy collision
void enemy_player_collision(Enemy* e, Player* pl, 
    Coin* coins, int coinLen,
    Message* msgs, int msgLen) {

    const float STOMP_POWER = 6.0f;
    const float PL_RADIUS = 16.0f;
    const float STOMP_WIDTH[] = {
        64, 64,
    };
    const float STOMP_Y[] = {
        -12, -4,
    };

    if (e->dying || !e->exist) 
        return;   

    // Check player explosion
    if (pl->exp.exist) {

        if (hypotf(pl->exp.pos.x - e->pos.x,  
            pl->exp.pos.y - e->pos.y) 
            < e->radius + exp_get_radius(&pl->exp)) {

            enemy_kill(e, pl->stats, coins, coinLen, false,
                msgs, msgLen);
        }

        return;
    }

    if (pl->dying || pl->respawnTimer > 0.0f)
        return;

    float w = STOMP_WIDTH[e->id];
    float y = e->pos.y + STOMP_Y[e->id];

    // Check stomp
    if (pl_jump_collision(pl,
        e->pos.x - w/2,
        y, w, STOMP_POWER)) {

        enemy_kill(e, pl->stats, coins, coinLen, true,
                msgs, msgLen);
    }


    if (pl->invincibilityTimer > 0.0f)
        return;

    // Check if inside a collision area
    if (hypotf(pl->pos.x - e->pos.x,  
        pl->pos.y - pl->spr.height/2 - e->pos.y) 
        < e->radius + PL_RADIUS) {

        pl_kill(pl, 1);
    }
}


// Draw an enemy
void enemy_draw(Enemy* e, Graphics* g) {
    
    const int DVALUE = 10;

    if (!e->exist) return;

    // Draw dying
    if (e->dying) {

        enemy_draw_dying(e, g);
        return;
    }

    int x = (int) roundf(e->pos.x);
    int y = (int) roundf(e->pos.y);

    // If hurt, use light colors
    int func = (e->hurtTimer <= 0.0f || 
        (int)floorf(e->hurtTimer/4) % 2 == 0)
        ? PixelFunctionDarken : PixelFunctionLighten;
    g_set_pixel_function(g, func, DVALUE, 0);

    // Draw sprite
    spr_draw(&e->spr, g, bmpEnemy, 
        x - e->spr.width/2, y - e->spr.height/2, false);

    g_set_pixel_function(g, PixelFunctionDefault, 0, 0);
}
