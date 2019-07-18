#include "dust.h"

#include <engine/mathext.h>

// Time
static const float DUST_TIME = 60.0f;


// Create dust
Dust create_dust(){
    
    Dust d;
    d.speed = 1.0f;
    d.timer = DUST_TIME;

    d.exist = false;

    return d;
}


// Activate
void dust_activate(Dust* d, Vector2 pos, float speed, 
    int sx, int sy, int sw, int sh) {

    d->exist = true;

    d->sx = sx;
    d->sy = sy;
    d->sw = sw;
    d->sh = sh;

    d->pos = pos;
    d->speed = speed;

    d->timer = DUST_TIME;
}


// Update dust
void dust_update(Dust* d, float tm){
    
    if (!d->exist) return;

    d->timer -= d->speed * tm;
    if (d->timer <= 0.0f) {

        d->exist = false;
    }
}


// Draw dust
void dust_draw(Dust* d, Graphics* g, Bitmap* bmp, uint8 col) {

    if (!d->exist) return;

    int skip = 1 + (int) floorf(d->timer / DUST_TIME * 12);

    float scale = d->timer / DUST_TIME;

    int dx = (int) (d->pos.x - d->sw*scale/2);
    int dy = (int) (d->pos.y - d->sh*scale/2);

    int dw = (int) (d->sw*scale);
    int dh = (int) (d->sh*scale);

    g_set_pixel_function(g, 
        PixelFunctionSingleColorSkipSimple, 
        skip, 0b11110100);
    g_draw_scaled_bitmap_region(g, bmp, 
        d->sx, d->sy, d->sw, d->sh, 
        dx, dy, dw, dh, false);
    g_set_pixel_function(g, PixelFunctionDefault, 0, 0);
}
