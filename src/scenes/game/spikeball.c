#include "spikeball.h"

#include <engine/mathext.h>

#include <math.h>

// Constants
static const float SPECIAL_AMPLITUDE = 24.0f;

// Global bitmaps
static Bitmap* bmpSpikeball;


// Initialize global content
void init_global_spikeballs(AssetManager* a) {

    // Get bitmaps
    bmpSpikeball = (Bitmap*)assets_get(a, "spikeball");
}


// Create a spikeball
Spikeball create_spikeball() {

    Spikeball b;
    b.exist = false;

    return b;
}


// Activate a spikeball
void sb_activate(Spikeball* b, float x, float maxY, int type) {

    const int X_VARY = 64.0f;
    const float SAFETY_RANGE = 32.0f;

    float mid = (maxY-24.0f) / 2.0f;
    int vary = (maxY-SAFETY_RANGE*2)/2;

    b->pos = vec2(
        x + (float)(rand() % X_VARY),
        mid + (rand() % (vary*2))-vary );
    // Make sure not too close
    if (b->pos.y+SAFETY_RANGE > maxY) {

        b->pos.y = maxY - SAFETY_RANGE;
        if (b->type == 1) {

            b->pos.y -= SPECIAL_AMPLITUDE;
        }

        // Not enough room, forget
        if (b->pos.y < SAFETY_RANGE) return;
    }
    // ...or too far
    else if(b->pos.y < SAFETY_RANGE) {

        b->pos.y = SAFETY_RANGE;
        if (b->type == 1) {

            b->pos.y += SPECIAL_AMPLITUDE;
        }
    }

    b->type = type;
    b->wave = (float)(rand() % 1000)/1000.0f * M_PI*2;
    b->startY = b->pos.y;
    b->maxY = maxY;

    b->exist = true;
}


// Update a spikeball
void sb_update(Spikeball* b, float globalSpeed, float tm) {

    const float CHECK_RADIUS = 24.0f;
    const float WAVE_SPEED = 0.05f;

    if (!b->exist) return;

    // If special, do waves
    if (b->type == 1) {

        b->wave += WAVE_SPEED * tm;
        b->wave = fmodf(b->wave, 2*M_PI);
        b->pos.y = b->startY + sinf(b->wave)*SPECIAL_AMPLITUDE;
    }

    // Move
    b->pos.x -= globalSpeed * tm;
    if (b->pos.x < -CHECK_RADIUS) {

        b->exist = false;
    }
}


// Spikeball-player collision
void sb_player_collision(Spikeball* b, Player* pl) {

    if (!b->exist) return;
}



// Draw a spikeball
void sb_draw(Spikeball* b, Graphics* g) {

    if (!b->exist) return;

    int x = (int) roundf(b->pos.x);
    int y = (int) roundf(b->pos.y);
    int i;

    // Draw chain
    for (i = 0; i < (192-b->pos.y)/32 +1; ++ i) {

        g_draw_bitmap_region(g, bmpSpikeball, 
            96, 0, 16, 32,
            x - 8, y - (i+1)*32, false);
    }

    // Draw sprite
    g_draw_bitmap_region(g, bmpSpikeball, 
        b->type*48, 0, 48, 48,
        x - 24, y - 24, false);
}
