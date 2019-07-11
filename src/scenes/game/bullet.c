#include "bullet.h"

#include <math.h>


// Create bullet
Bullet create_bullet()  {

    Bullet b;
    b.exist = false;

    return b;
}



// Activate a bullet
void bullet_activate(Bullet* b, Vector2 pos, Vector2 speed, float radius) {

    b->pos = pos;
    b->speed = speed;
    b->angle = 0.0f;
    b->radius = radius;

    b->exist = true;
}


// Update a bullet
void bullet_update(Bullet* b, float tm) {

    const float ROTATE_SPEED = 0.1f;
    const float ROTATE_COMPARE = 4.0f;
    const float FLOOR_HEIGHT = 16;

    if (!b->exist) return;

    // Move
    b->pos.x += b->speed.x * tm;
    b->pos.y += b->speed.y * tm;

    // Check if inside the screen boundaries
    if (b->pos.x+b->radius < 0 || b->pos.x-b->radius > 256 ||
        b->pos.y+b->radius < 0 || b->pos.y+b->radius > 192-FLOOR_HEIGHT) {

        b->exist = false;
    }

    b->angle += b->speed.x/ROTATE_COMPARE * ROTATE_SPEED * tm;
}



// Draw a bullet
void bullet_draw(Bullet* b, Graphics* g) {

    if (!b->exist) return;

    float step = M_PI * 2.0f / 3.0f;

    float x1 = b->pos.x + cosf(b->angle) * b->radius;
    float y1 = b->pos.y + sinf(b->angle) * b->radius;

    float x2 = b->pos.x + cosf(b->angle + step) * b->radius;
    float y2 = b->pos.y + sinf(b->angle + step) * b->radius;

    float x3 = b->pos.x + cosf(b->angle + step*2) * b->radius;
    float y3 = b->pos.y + sinf(b->angle + step*2) * b->radius;

    g_set_pixel_function(g, PixelFunctionInverse, 0, 0);
    g_draw_triangle(
        g, (int)x1, (int)y1,
        (int)x2, (int)y2,
        (int)x3, (int)y3,
        0
    );
    g_set_pixel_function(g, PixelFunctionDefault, 0, 0);
}

