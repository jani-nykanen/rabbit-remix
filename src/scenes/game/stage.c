#include "stage.h"

#include <math.h>


// Create stage
Stage create_stage(AssetManager* a) {

    Stage s;

    // Get bitmaps
    s.bmpFence = (Bitmap*)assets_get(a, "fence");
    s.bmpBush = (Bitmap*)assets_get(a, "bush");
    s.bmpHouses = (Bitmap*)assets_get(a, "houses");
    s.bmpSky = (Bitmap*)assets_get(a, "sky");
    
    // Set initial values
    s.fencePos = 0.0f;
    s.housePos = 0.0f;
    s.fencePos = 0.0f;
    s.cloudPos = 0.0f;

    return s;
}


// Update stage
void stage_update(Stage* s, float globalSpeed,  float tm) {

    const float BUSH_SPEED = 0.5f;
    const float HOUSE_SPEED = BUSH_SPEED / 2.0f;
    const float FENCE_SPEED = 1.0f;

    //
    // Update positions
    //
    s->fencePos += FENCE_SPEED * globalSpeed * tm;
    s->fencePos = fmodf(s->fencePos, s->bmpFence->width);

    s->bushPos += BUSH_SPEED * globalSpeed * tm;
    s->bushPos = fmodf(s->bushPos, 128.0f);

    s->housePos += HOUSE_SPEED * globalSpeed * tm;
    s->housePos = fmodf(s->housePos, 256.0f);
}


// Draw stage
void stage_draw(Stage* s, Graphics* g) {

    const int GROUND_HEIGHT = 40;
    const int BUSH_Y = 80;
    const int HOUSE_Y = 28;
    const int FENCE_Y = 192 - GROUND_HEIGHT - s->bmpFence->height;

    int i, p;

    // Sky
    g_draw_bitmap_fast(g, s->bmpSky, 0, 0);

    // Houses
    p = (int) floorf(s->housePos);
    for (i = 0; i < 2; ++ i) {

        g_draw_bitmap(g, s->bmpHouses, 
            -p + i * s->bmpHouses->width, HOUSE_Y, 
            false);
    }
    // Bushes
    p = (int) floorf(s->bushPos);
    for (i = 0; i < 3; ++ i) {

        g_draw_bitmap(g, s->bmpBush,  
            -p + s->bmpBush->width*i, BUSH_Y, 
            false);
    }
    // Fence
    p = (int) floorf(s->fencePos);
    for (i = 0; i < 256/s->bmpFence->width +2; ++ i) {

        g_draw_bitmap(g, s->bmpFence,  
            -p + s->bmpFence->width*i, FENCE_Y, 
            false);
    }

    // The ground will come here
    g_fill_rect(g, 0, 
        192-GROUND_HEIGHT, 
        256, 48, 
        0b00001000);
}
