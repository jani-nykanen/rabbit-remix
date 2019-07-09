#include "stage.h"


// Create stage
Stage create_stage(AssetManager* a) {

    Stage s;

    // Get bitmaps
    s.bmpBush = (Bitmap*)assets_get(a, "bush");
    s.bmpHouses = (Bitmap*)assets_get(a, "houses");
    s.bmpSky = (Bitmap*)assets_get(a, "sky");

    return s;
}


// Update stage
void stage_update(Stage* s, float tm) {

    // ...
}


// Draw stage
void stage_draw(Stage* s, Graphics* g) {

    int i;

    // Sky
    g_draw_bitmap_fast(g, s->bmpSky, 0, 0);

    // Houses
    g_draw_bitmap(g, s->bmpHouses, 0, 32, false);
    // Bushes
    for (i = 0; i < 2; ++ i) {

        g_draw_bitmap(g, s->bmpBush, s->bmpBush->width*i, 96, false);
    }
}
