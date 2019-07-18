#include "spikeball.h"

#include "stage.h"

#include <engine/mathext.h>

#include <math.h>

// Constants
static const float SPECIAL_AMPLITUDE = 24.0f;

// Global bitmaps
static Bitmap* bmpSpikeball;
static Bitmap* bmpChain;


// Initialize global content
void init_global_spikeballs(AssetManager* a) {

    // Get bitmaps
    bmpSpikeball = (Bitmap*)assets_get(a, "spikeball");
    bmpChain = (Bitmap*)assets_get(a, "chain");
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
    const int TOP_OFF = 64.0f;
    const float SAFETY_RANGE = 20.0f;

    float mid = maxY-24.0f;
    int vary = (maxY-TOP_OFF);

    b->pos = vec2(
        x + (float)(rand() % X_VARY),
        mid - (rand() % (vary)) );
    // Make sure not too close
    if (b->pos.y+SAFETY_RANGE > maxY) {

        b->pos.y = maxY - SAFETY_RANGE;
        if (b->type == 1) {

            b->pos.y -= SPECIAL_AMPLITUDE;
        }

        // Not enough room, forget
        if (b->pos.y < TOP_OFF) return;
    }
    // ...or too far
    else if(b->pos.y < TOP_OFF) {

        b->pos.y = TOP_OFF;
        if (b->type == 1) {

            b->pos.y += SPECIAL_AMPLITUDE;
        }
    }

    b->type = type;
    b->wave = (float)(rand() % 1000)/1000.0f * M_PI*2;
    b->startPos = b->pos;
    b->maxY = maxY;
    b->speed = vec2(0, 0);
    b->maxDist = b->pos.y;
    b->startDist = b->maxDist;

    b->exist = true;
}


// Update a spikeball
void sb_update(Spikeball* b, float globalSpeed, float tm) {

    const float CHECK_RADIUS = 24.0f;
    const float WAVE_SPEED = 0.05f;
    const float EPS = 0.1f;
    const float SPEED_DELTA = 0.05f;


    if (!b->exist) return;

    float dist = hypotf(b->pos.x-b->startPos.x, b->pos.y);
    bool stopped = fabsf(dist - b->maxDist) < EPS &&
        hypotf(b->speed.x, b->speed.y) < EPS;

    // If special, do waves
    if (b->type == 1 && stopped) {

        b->wave += WAVE_SPEED * tm;
        b->wave = fmodf(b->wave, 2*M_PI);
        b->maxDist = b->startDist + sinf(b->wave)*SPECIAL_AMPLITUDE;
    }
    

    // Update speed
    if (b->pos.x > b->startPos.x+EPS) {

        b->speed.x -= SPEED_DELTA * tm;
    }
    else if (b->pos.x < b->startPos.x-EPS) {

        b->speed.x += SPEED_DELTA * tm;
    }

    // Move
    b->pos.x += ((-globalSpeed) + b->speed.x) * tm;
    b->startPos.x -= globalSpeed * tm;
    if (b->startPos.x < -CHECK_RADIUS &&
        b->pos.x < -CHECK_RADIUS) {

        b->exist = false;
    }
    if (fabsf(b->pos.x-b->startPos.x) < EPS) {

        b->pos.x = b->startPos.x;
    }

    // Check if moved too far
    float angle = atan2f(b->pos.y, b->pos.x-b->startPos.x);
    float dx, dy;
    if (fabsf(dist - b->maxDist) > EPS) {

        dx = cosf(angle) * (b->maxDist - dist);
        dy = sinf(angle) * (b->maxDist - dist);

        b->pos.x += dx;
        b->pos.y += dy;
    }
}


// Spikeball-player collision
void sb_player_collision(Spikeball* b, Player* pl) {

    const float SELF_RADIUS = 16.0f;
    const float PL_RADIUS = 16.0f;

    if (!b->exist && !pl->dying && pl->respawnTimer > 0.0f) return;

    float px = pl->pos.x;
    float py = pl->pos.y - pl->spr.height/2;
    float bx = b->pos.x;
    float by = b->pos.y;

    if (hypotf(px - bx, py - by) < SELF_RADIUS + PL_RADIUS) {

        pl_kill(pl, 1);
    }
}


// Spikeball-bullet collision
void sb_bullet_collision(Spikeball* sb, Bullet* b) {

    const float RADIUS= 12.0f;
    const float SPEED_MUL = 2.0f;
    const float SPEED_COMP = 12.0f;

    if (!sb->exist || !b->exist) return;

    // Check if inside the collision area
    if (hypotf(b->pos.x-sb->pos.x, b->pos.y-sb->pos.y) < RADIUS+b->radius) {

        bullet_kill(b);
        sb->speed.x = b->radius/SPEED_COMP * SPEED_MUL;
    }
}


// Draw the spikeball shadow
void sb_draw_shadow(Spikeball* b, Graphics* g) {

    const float SHADOW_SCALE_COMPARE = 176+128;
    const float COMPARE_DELTA = 128;
    const int SHADOW_MOVE_Y = 3;
    const int SHADOW_DARK_VALUE = 5;

    if (!b->exist) return;

    int px = (int)roundf(b->pos.x);
    int py = (int)roundf(b->pos.y);

    // Draw shadow
    int scale = (int)((b->pos.y+COMPARE_DELTA)/
        SHADOW_SCALE_COMPARE *48.0f);

    g_set_pixel_function(g, 
        PixelFunctionDarken, 
        SHADOW_DARK_VALUE, 0);
    g_draw_scaled_bitmap_region(g, bmpSpikeball,
        96, 0, 48, 48, 
        px-scale/2, 
        192-GROUND_COLLISION_HEIGHT+SHADOW_MOVE_Y - scale, 
        scale, scale, false);
    g_set_pixel_function(g, PixelFunctionDefault, 0, 0);
}


// Draw chain
static void draw_chain(Graphics* g, 
    int dx1, int dy1, int dx2, int dy2, int r) {

    float angle = atan2f(dy2-dy1, dx2-dx1) + M_PI/2.0f;
    float len = hypotf(dx2-dx1, dy2-dy1);
    float texRepeat = len / (float)bmpChain->height;

    float c = cosf(angle);
    float s = sinf(angle);

    int x1 = dx1-(int)(c*r/2);
    int y1 = dy1-(int)(s*r/2);

    int x2 = dx2-(int)(c*r/2);
    int y2 = dy2-(int)(s*r/2);

    int x3 = dx2+(int)(c*r/2);
    int y3 = dy2+(int)(s*r/2);

    int x4 = dx1+(int)(c*r/2);
    int y4 = dy1+(int)(s*r/2);

    g_toggle_texturing(g, bmpChain);

    g_set_uv_coords(g, 0, 0, 0, texRepeat, 1.0f, texRepeat);
    g_draw_triangle(g, x1, y1, x2, y2, x3, y3, 0);

    g_set_uv_coords(g, 1.0f, texRepeat, 1.0f, 0, 0, 0);
    g_draw_triangle(g, x3, y3, x4, y4, x1, y1, 0);

    g_toggle_texturing(g, NULL);
} 


// Draw a spikeball
void sb_draw(Spikeball* b, Graphics* g) {

    const int ROPE_WIDTH = 24;
    const uint8 COLORS[] = {
        0b01000100,
        0b10101000,
        0b11110101,
    };

    if (!b->exist) return;
    
    int x = (int) roundf(b->pos.x);
    int y = (int) roundf(b->pos.y);
    
    int i;

    /*for (i = 0; i < 3; ++ i) {
        
        g_draw_thick_line(g, 
            (int)roundf(b->startPos.x), 
            -ROPE_WIDTH/2, x, y, 
            ROPE_WIDTH - i*2, 
            COLORS[i]);
    }
    */
   // Draw chain
   draw_chain(g, 
            (int)roundf(b->startPos.x), 
            -ROPE_WIDTH/2, x, y, 
            ROPE_WIDTH);


    // Draw sprite
    g_draw_bitmap_region(g, bmpSpikeball, 
        b->type*48, 0, 48, 48,
        x - 24, y - 24, false);
}
