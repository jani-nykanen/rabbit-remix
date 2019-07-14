#include "mushroom.h"

// Bitmaps
static Bitmap* bmpMushroom;

// Constants
static const float BOUNCE_WAIT = 20.0f;


// Initialize global content
void init_global_mushrooms(AssetManager* a) {

    bmpMushroom = (Bitmap*)assets_get(a, "mushrooms");
}


// Create a mushroom
Mushroom create_mushroom() {

    Mushroom m;
    m.exist = false;

    return m;
}


// Activate a mushroom
float mush_activate(Mushroom* m, Vector2 pos, int major, int minor) {

    const float WAIT_MOD[] = {
        1.0f, 1.25f, 1.0f
    };

    const int WIDTHS[] = {
        64, 96, 64,
    };
    const int HEIGHTS[] = {
        32, 32, 64
    };
    const int ROW[] = {
        0, 1, 1
    };

    m->pos = pos;
    m->majorType = major;
    m->minorType = minor;

    m->exist = true;
    
    // Set initials
    m->bounceTimer = 0.0f;

    // Set dimensions
    int w = WIDTHS[major];
    int h = HEIGHTS[major];
    m->spr = create_sprite(w, h);
    m->spr.row = ROW[m->majorType];
    m->spr.frame = m->minorType * 2;

    return WAIT_MOD[major];
}


// Update a mushroom
void mush_update(Mushroom* m, float globalSpeed, float tm) {

    const float STOP_RADIUS = 48;

    if (!m->exist) return;

    // Update bounce timer
    if (m->bounceTimer > 0.0) {

        m->bounceTimer -= 1.0f * tm;
        m->spr.frame = m->minorType * 2 +1;
    }
    else {

        m->spr.frame = m->minorType * 2;
    }

    // Move
    m->pos.x -= globalSpeed * tm;
    if (m->pos.x+STOP_RADIUS < 0) {

        m->exist = false;
    }
}


// Player collision
void mush_player_collision(Mushroom* m, Player* pl) {

    const float HEIGHT_MUL = 0.70f;
    const float SPEED_BASE = 3.0f;
    const float POWERS[] = {
        7.0f, 6.0f, 8.0f,
    };

    if (!m->exist) return;

    float mul = pl->speed.y / SPEED_BASE;

    if (pl_jump_collision(
        pl,
        m->pos.x-m->spr.width/2.0f,
        m->pos.y-m->spr.height * HEIGHT_MUL,
        m->spr.width,
        POWERS[m->majorType] )) {
            
        m->bounceTimer = BOUNCE_WAIT * mul;
    }
}


// Draw a mushroom
void mush_draw(Mushroom* m, Graphics* g) {

    const int SHADOW_Y_OFF = 6;
    const int SHADOW_DARK_VALUE = 5;
    const float SCALE_MOD = 0.2f;

    if (!m->exist) return;

    int px = (int) roundf(m->pos.x-m->spr.width/2);
    int py = (int) roundf(m->pos.y-m->spr.height);
    int oy = py;
    int sx, sy;
    float t;

    // Draw base mushroom
    if (m->bounceTimer > 0.0f) {

        t = m->bounceTimer / BOUNCE_WAIT;
        sx = (int) (m->spr.width* (1.0f+t*SCALE_MOD));
        sy = (int) (m->spr.height*(1.0f-t*SCALE_MOD));

        px -= (sx - m->spr.width) / 2; 
        py -= (sy - m->spr.height);

        // Draw scaled shadow
        g_set_pixel_function(g, 
            PixelFunctionDarken, 
            SHADOW_DARK_VALUE, 0);
        spr_draw_scaled_frame(&m->spr, g, bmpMushroom, 
            px, 
            oy+SHADOW_Y_OFF, 
            2, m->spr.row, sx, m->spr.height, false);
        g_set_pixel_function(g, PixelFunctionDefault, 0, 0);

        // Draw scaled sprite
        spr_draw_scaled(&m->spr, 
            g, bmpMushroom, px, py, 
            sx, sy, false);
    }
    else {

        // Draw shadow
        g_set_pixel_function(g, 
            PixelFunctionDarken, 
            SHADOW_DARK_VALUE, 0);
        spr_draw_frame(&m->spr, g, bmpMushroom, 
            px, 
            py+SHADOW_Y_OFF, 
            2, m->spr.row, false);
        g_set_pixel_function(g, PixelFunctionDefault, 0, 0);

        // Draw sprite
        spr_draw(&m->spr, g, bmpMushroom, px, py, false);
    }
}
