#include "mushroom.h"

#include <engine/mathext.h>

#include <math.h>

// Bitmaps
static Bitmap* bmpMushroom;

// Constants
static const float BOUNCE_WAIT = 20.0f;


// Initialize global content
void init_global_mushrooms(AssetManager* a) {

    bmpMushroom = (Bitmap*)assets_get(a, "mushrooms");
}


// Update jump
static void mush_update_jump(Mushroom* m, float tm) {

    const float MAX_GRAVITY = 3.0f;
    const float GRAVITY_DELTA = 0.15f;
    const float JUMP_WAIT = 30.0f;
    const float JUMP_HEIGHT_MAX = -4.5f;
    const float JUMP_HEIGHT_MIN = -3.5f;

    if (m->majorType == 3) {

        // Waiting for jump
        if (m->jumpTimer > 0.0f) {

            m->spr.frame = 0;

            m->jumpTimer -= 1.0f * tm;
            if (m->jumpTimer <= 0.0f) {

                // Determine jump height
                m->gravity = 
                    ((float)(rand() % 100))/100.0f * 
                    (JUMP_HEIGHT_MAX-JUMP_HEIGHT_MIN) + JUMP_HEIGHT_MIN;
            }
        }
        // Jumping
        else {

            m->gravity += GRAVITY_DELTA * tm;
            if (m->gravity > MAX_GRAVITY) {

                m->gravity = MAX_GRAVITY;
            }
            m->pos.y += m->gravity * tm;

            // Determine frame
            m->spr.frame = m->gravity < 0.0f ? 1 : 2;

            // Check floor
            if (m->pos.y > m->startPos.y) {

                m->pos.y = m->startPos.y;
                m->jumpTimer = JUMP_WAIT;
            }
        }
    }
}


// Create a mushroom
Mushroom create_mushroom() {

    Mushroom m;
    m.exist = false;

    return m;
}


// Activate a mushroom
float mush_activate(Mushroom* m, Vector2 pos, int major, int minor) {

    const int JUMP_WAIT_VARY = 60;

    const float WAIT_MOD[] = {
        1.0f, 1.25f, 1.0f, 1.0f,
    };

    const int WIDTHS[] = {
        64, 96, 64, 64,
    };
    const int HEIGHTS[] = {
        32, 32, 64, 32
    };
    const int ROW[] = {
        0, 1, 1, 4,
    };

    m->pos = pos;
    m->startPos = pos;
    m->majorType = major;
    m->minorType = minor;

    m->exist = true;
    
    // Set initials
    m->bounceTimer = 0.0f;
    m->jumpTimer = (float) (rand() % JUMP_WAIT_VARY);
    m->gravity = 0.0f;

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

    // Jumping
    mush_update_jump(m, tm);

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
        7.0f, 9.0f, 6.0f, 7.0f,
    };

    if (!m->exist) return;

    float mul = pl->speed.y / SPEED_BASE;

    if (pl_jump_collision(
        pl,
        m->pos.x-m->spr.width/2.0f,
        m->pos.y-m->spr.height * HEIGHT_MUL,
        m->spr.width,
        POWERS[m->majorType] - min_float_2(0.0f, m->gravity) )) {
            
        m->bounceTimer = BOUNCE_WAIT * mul;
        m->gravity *= -1;
    }
}


// Draw a mushroom
void mush_draw(Mushroom* m, Graphics* g) {

    //
    // TODO: CLEAN UP THIS HORRID MESS!
    // Too much repeating code and other weird things.
    // It makes me want to cry.
    // 

    const int SHADOW_Y_OFF = 6;
    const int SHADOW_DARK_VALUE = 5;
    const float SCALE_MOD = 0.2f;
    const int SHADOW_FRAME[] = {
        2, 2, 2, 3,
    };
    const float Y_COMP = 128.0f;
    const float EPS = 0.01f;

    if (!m->exist) return;

    int px = (int) roundf(m->pos.x-m->spr.width/2);
    int py = (int) roundf(m->pos.y-m->spr.height);
    int oy = (int) roundf(m->startPos.y-m->spr.height);
    int sx, sy;
    int shadowScaleX, shadowScaleY;
    float st = 1.0f - fabsf(m->pos.y - m->startPos.y) / Y_COMP;
    float t;

    // Draw base mushroom
    if (m->bounceTimer > 0.0f) {

        t = m->bounceTimer / BOUNCE_WAIT;
        sx = (int) (m->spr.width* (1.0f+t*SCALE_MOD));
        sy = (int) (m->spr.height*(1.0f-t*SCALE_MOD));

        shadowScaleX = (int) (m->spr.width  * st) * (1.0f+t*SCALE_MOD);
        shadowScaleY = (int) (m->spr.height * st);

        px -= (sx - m->spr.width) / 2; 
        py -= (sy - m->spr.height);

        // Draw scaled shadow
        g_set_pixel_function(g, 
            PixelFunctionDarken, 
            SHADOW_DARK_VALUE, 0);
        spr_draw_scaled_frame(&m->spr, g, bmpMushroom, 
            px - (shadowScaleX - m->spr.width) / 2, 
            oy - (shadowScaleY - m->spr.height) + SHADOW_Y_OFF*st, 
            SHADOW_FRAME[m->majorType], 
            m->spr.row, shadowScaleX, shadowScaleY, false);
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
        if (fabsf(t-1.0f) < EPS) {

            spr_draw_frame(&m->spr, g, bmpMushroom, 
                px, 
                oy+SHADOW_Y_OFF, 
                SHADOW_FRAME[m->majorType],  
                m->spr.row, false);
        }
        else {  

            shadowScaleX = (int) (m->spr.width  * st);
            shadowScaleY = (int) (m->spr.height * st);

            spr_draw_scaled_frame(&m->spr, g, bmpMushroom, 
                px - (shadowScaleX - m->spr.width) / 2, 
                oy - (shadowScaleY - m->spr.height) + SHADOW_Y_OFF*st, 
                SHADOW_FRAME[m->majorType], 
                m->spr.row, shadowScaleX, shadowScaleY, false);
        }

        g_set_pixel_function(g, PixelFunctionDefault, 0, 0);

        // Draw sprite
        spr_draw(&m->spr, g, bmpMushroom, px, py, false);
    }
}
