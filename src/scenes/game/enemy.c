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


// Kill
static void enemy_kill(Enemy* e, Stats* s, 
    Coin* coins, int coinLen, bool stomped) {

    const float POWER_PLUS = 0.25f;

    e->deathTimer = DEATH_TIME;
    e->dying = true;

    stats_modify_power(s, POWER_PLUS);
}


// Update special behavior
static void enemy_update_special(Enemy* e, float globalSpeed, float tm) {

    const float WAVE_SPEED_X = 0.025f;
    const float WAVE_SPEED_Y = 0.05f;

    const float AMPLITUDE_X = 0.5f;
    const float AMPLITUDE_Y = 12.0f;

    switch (e->id)
    {
    case 0:

        // Waves
        if (e->hurtTimer <= 0.0f) {
            
            e->wave.x += WAVE_SPEED_X * tm;
            e->speed.x = 1.0f + sinf(e->wave.x) * AMPLITUDE_X;
        }

        e->wave.y += WAVE_SPEED_Y * tm;
        e->pos.y = e->startPos.y + sinf(e->wave.y) * AMPLITUDE_Y;
        
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
        16.0f
    };
    const int HIT_POINTS[] = {
        2
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

    switch(e->id) {

    case 0:
        e->speed.x = 0.5f;
        break;

    default:
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
    Enemy* e, Bullet* b, Stats* s, Coin* coins, int coinLen){
        
    const float HURT_TIME = 30.0f;

    if (!e->exist || !b->exist || b->dying || e->dying) 
        return;

    // Check if inside the collision area
    if (hypotf(b->pos.x-e->pos.x, b->pos.y-e->pos.y) < e->radius+b->radius) {

        bullet_kill(b);

        if (--e->health <= 0) {

            enemy_kill(e, s, coins, coinLen, false);
        }
        else {

            e->hurtTimer = HURT_TIME;
            e->wave.x = fmodf(e->wave.x, M_PI);
        }
    }
}


// Player-enemy collision
void enemy_player_collision(Enemy* e, Player* pl, 
    Coin* coins, int coinLen) {

    const float STOMP_POWER = 6.0f;
    const float PL_RADIUS = 16.0f;
    const float STOMP_WIDTH[] = {
        64,
    };
    const float STOMP_Y[] = {
        -12
    };
    const float SELF_RADIUS[] = {
        16.0f
    };

    if (e->dying || !e->exist || 
        pl->dying || pl->respawnTimer > 0.0f) 
        return;   

    float w = STOMP_WIDTH[e->id];
    float y = e->pos.y + STOMP_Y[e->id];

    // Check stomp
    if (pl_jump_collision(pl,
        e->pos.x - w/2,
        y, w, STOMP_POWER)) {

        enemy_kill(e, pl->stats, coins, coinLen, true);
    }


    if (pl->invincibilityTimer > 0.0f)
        return;

    // Check if inside a collision area
    if (hypotf(pl->pos.x - e->pos.x,  
        pl->pos.y - pl->spr.height/2 - e->pos.y) 
        < SELF_RADIUS[e->id] + PL_RADIUS) {

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
