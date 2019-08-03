#include "bullet.h"

#include <math.h>

// Samples
static Sample* sBulletHit;

// Death time
static const float DEATH_TIME = 15.0f;


// Initialize global bullet content
void init_global_bullets(AssetManager* a) {

    // Get samples
    sBulletHit = (Sample*)assets_get(a, "bulletHit");
}


// Create bullet
Bullet create_bullet()  {

    Bullet b;
    b.exist = false;

    return b;
}


// Activate a bullet
void bullet_activate(Bullet* b, Vector2 pos, Vector2 speed, 
    float radius, bool spc) {

    b->pos = pos;
    b->speed = speed;
    b->angle = 0.0f;
    b->radius = radius;
    b->isSpecial = spc;
    b->hitPlayed = false;

    b->exist = true;
}


// Update a bullet
void bullet_update(Bullet* b, EventManager* evMan, float tm) {

    const float ROTATE_SPEED = 0.1f;
    const float ROTATE_COMPARE = 4.0f;
    const float FLOOR_HEIGHT = 16;

    if (!b->exist) return;

    if (b->dying) {

        if (!b->hitPlayed) {

            audio_play_sample(evMan->audio, sBulletHit, 0.70f, 0);
            b->hitPlayed = true;
        }

        if ((b->deathTimer -= 1.0f * tm) <= 0.0f) {

            b->exist = false;
            b->dying = false;
        }
        return;
    }

    // Move
    b->pos.x += b->speed.x * tm;
    b->pos.y += b->speed.y * tm;

    // Check if inside the screen boundaries
    if (b->pos.x+b->radius < 0 || b->pos.x-b->radius > 256 ||
        b->pos.y+b->radius < 0 || b->pos.y > 192-FLOOR_HEIGHT) {

        b->exist = false;
    }

    b->angle += b->speed.x/ROTATE_COMPARE * ROTATE_SPEED * tm;
}



// Draw a bullet
void bullet_draw(Bullet* b, Graphics* g) {

    const float DEATH_RADIUS = 1.0f;

    if (!b->exist) return;

    float t = b->radius;
    int param1 = 0;
    if (b->dying) {

        t *= 1.0f + (1.0f-b->deathTimer/DEATH_TIME)*DEATH_RADIUS;
        param1 = 1 + (int) floorf(b->deathTimer / DEATH_TIME * 12);
    }

    float step = M_PI * 2.0f / 3.0f;

    float x1 = b->pos.x + cosf(b->angle) * t;
    float y1 = b->pos.y + sinf(b->angle) * t;

    float x2 = b->pos.x + cosf(b->angle + step) * t;
    float y2 = b->pos.y + sinf(b->angle + step) * t;

    float x3 = b->pos.x + cosf(b->angle + step*2) * t;
    float y3 = b->pos.y + sinf(b->angle + step*2) * t;

    g_set_pixel_function(g, 
        b->dying ? PixelFunctionInverseSkip : PixelFunctionInverseFrame, 
        param1, 0);
    g_draw_triangle(
        g, (int)x1, (int)y1,
        (int)x2, (int)y2,
        (int)x3, (int)y3,
        0
    );
    g_set_pixel_function(g, PixelFunctionDefault, 0, 0);
}


// Kill a bullet
void bullet_kill(Bullet* b) {

    if (!b->exist || b->dying) return;

    b->dying = true;
    b->deathTimer = DEATH_TIME;
}
