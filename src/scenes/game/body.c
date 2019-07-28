#include "body.h"


#include <math.h>

// Create a body
Body create_body() {

    Body b;
    b.exist = false;

    return b;
}


// Activate body
void body_activate(Body* b, Vector2 pos, Sprite spr) {

    b->pos = pos;
    b->spr = spr;
    b->exist = true;
}


// Update body
void body_update(Body* b, float globalSpeed, float tm) {

    if (!b->exist) return;

    // Move
    b->pos.x -= globalSpeed * tm;
    if (b->pos.x < -b->spr.width/2) {

        b->exist = false;
    }
}


// Draw a body
void body_draw(Body* b, Graphics* g, Bitmap* bmp) {

    const int SHADOW_DARK_VALUE = 5;
    const int SHADOW_MOVE_Y = 3;

    if (!b->exist) return;

    int px = (int)roundf(b->pos.x)-b->spr.width/2;
    int py = (int)roundf(b->pos.y)-b->spr.height;

    // Draw shadow
    g_set_pixel_function(g, 
        PixelFunctionDarken, 
        SHADOW_DARK_VALUE, 0);
    g_draw_bitmap_region(g, bmp,
        144, 0, 48, 48, 
        px, py+SHADOW_MOVE_Y, false);
    g_set_pixel_function(g, PixelFunctionDefault, 0, 0);

    // Draw sprite
    spr_draw(&b->spr, g, bmp, 
        px,
        py, false);
}
