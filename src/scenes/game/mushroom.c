#include "mushroom.h"

// Bitmaps
static Bitmap* bmpMushroom;


// Initialize global content
void init_global_mushrooms(AssetManager* a) {

    bmpMushroom = (Bitmap*)assets_get(a, "mushroom");
}



// Create a mushroom
Mushroom create_mushroom() {

    Mushroom m;
    m.exist = false;

    return m;
}


// Activate a mushroom
void mush_activate(Mushroom* m, Vector2 pos, int major, int minor) {

    m->pos = pos;
    m->majorType = major;
    m->minorType = minor;

    m->exist = true;
    
    int w = 64;
    int h = 32;

    m->spr = create_sprite(w, h);
}


// Update a mushroom
void mush_update(Mushroom* m, float globalSpeed, float tm) {

    const float STOP_RADIUS = 48;

    if (!m->exist) return;

    // Move
    m->pos.x += globalSpeed * tm;
    if (m->pos.x+STOP_RADIUS < 0) {

        m->exist = false;
    }
}


// Draw a mushroom
void mush_draw(Mushroom* m, Graphics* g) {

    if (!m->exist) return;

    int px = (int) (m->pos.x-m->spr.width/2);
    int py = (int) (m->pos.y-m->spr.height);

    spr_draw(&m->spr, g, bmpMushroom, px, py, false);
}
