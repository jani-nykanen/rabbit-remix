#include "explosion.h"

#include <math.h>

// Constants
static const float EXP_TIME = 30.0f;


// Create an explosion
Explosion create_explosion() {

    Explosion e;

    e.exist = false;

    return e;
}


// Activate an explosion
void exp_activate(Explosion* e, Vector2 pos, float radius) {

    e->pos = pos;
    e->radius = radius;
    e->timer = EXP_TIME;

    e->exist = true;
}


// Update an explosion
void exp_update(Explosion* e, float tm) {

    if (!e->exist) return;

    if ((e->timer -= 1.0f * tm) <= 0.0f) {

        e->exist = false;
    }
}


// Draw an explosion
void exp_draw(Explosion* e, Graphics* g) {

    const int DIV = 16;
    
    if (!e->exist) return;

    int x = (int) e->pos.x;
    int y = (int) e->pos.y;

    int i;
    float angle;
    float step = 2*M_PI / (float)DIV;

    float t = e->timer / EXP_TIME;
    float r = e->radius * (1.0f-t);

    float lvalue = (int)floorf(t * 14);

    // Draw a filled circle, yay
    g_set_pixel_function(g, PixelFunctionLighten, lvalue, 0);
    for (i = 0; i < DIV; ++ i) {
        
        angle = i * step;
        g_draw_triangle(g,
            x + (int)(cosf(angle) * r),
            y + (int)(sinf(angle) * r),
            x + (int)(cosf((i+1)*step) * r),
            y + (int)(sinf((i+1)*step) * r),
            x, y, 255
            );
    }
    g_set_pixel_function(g, PixelFunctionDefault, 0, 0);
}
