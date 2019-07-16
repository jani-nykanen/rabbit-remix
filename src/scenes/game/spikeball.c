#include "spikeball.h"

#include <engine/mathext.h>

#include <math.h>

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

    const float HEIGHT_VARY = 32.0f;

    float mid = (maxY-24.0f) / 2.0f;

    b->pos = vec2(x, mid);
    b->type = type;

    b->exist = true;
}


// Update a spikeball
void sb_update(Spikeball* b, float globalSpeed, float tm) {

    const float CHECK_RADIUS = 24.0f;

    if (!b->exist) return;

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

    // Draw chain
    // TODO

    // Draw sprite
    g_draw_bitmap_region(g, bmpSpikeball, 
        b->type*48, 0, 48, 48,
        x - 24, y - 24, false);
}
