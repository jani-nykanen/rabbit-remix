#include "stage.h"

#include <math.h>


// Draw a scrolling element
static void draw_scrolling_element(Graphics* g, Bitmap* bmp, 
    float pos, int y) {
    
    int i, p;

    // Houses
    p = (int) floorf(pos);
    for (i = 0; i < 256/bmp->width +2; ++ i) {

        g_draw_bitmap(g, bmp, 
            -p + i * bmp->width, y, 
            false);
    }
}


// Update a scrolling element
static void update_scrolling_element(
    float* pos, float speed, int width, float tm) {

    *pos += speed * tm;
    *pos = fmodf(*pos, width);
}


// Create stage
Stage create_stage(AssetManager* a) {

    Stage s;

    // Get bitmaps
    s.bmpFence = (Bitmap*)assets_get(a, "fence");
    s.bmpBush = (Bitmap*)assets_get(a, "bush");
    s.bmpHouses = (Bitmap*)assets_get(a, "houses");
    s.bmpSky = (Bitmap*)assets_get(a, "sky");
    s.bmpClouds = (Bitmap*)assets_get(a, "clouds");
    
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
    const float CLOUD_SPEED = 0.33f;

    //
    // Update positions
    //
    update_scrolling_element(&s->fencePos, 
        FENCE_SPEED*globalSpeed, s->bmpFence->width, tm);
    update_scrolling_element(&s->bushPos, 
        BUSH_SPEED*globalSpeed, s->bmpBush->width, tm);
    update_scrolling_element(&s->housePos, 
        HOUSE_SPEED*globalSpeed, s->bmpHouses->width, tm);
    update_scrolling_element(&s->cloudPos, 
        CLOUD_SPEED*globalSpeed, s->bmpClouds->width, tm);    
}


// Draw stage
void stage_draw(Stage* s, Graphics* g) {

    const int GROUND_HEIGHT = 40;
    const int BUSH_Y = 80;
    const int HOUSE_Y = 28;
    const int FENCE_Y = 192 - GROUND_HEIGHT - s->bmpFence->height;
    const int CLOUD_Y = 8;

    int i, p;

    // Sky
    g_draw_bitmap_fast(g, s->bmpSky, 0, 0);

    // Clouds
    draw_scrolling_element(g, s->bmpClouds, s->cloudPos, CLOUD_Y);
    // Houses
    draw_scrolling_element(g, s->bmpHouses, s->housePos, HOUSE_Y);
    // Bushes
    draw_scrolling_element(g, s->bmpBush, s->bushPos, BUSH_Y);
    // Fence
    draw_scrolling_element(g, s->bmpFence, s->fencePos, FENCE_Y);

    // The ground will come here
    g_fill_rect(g, 0, 
        192-GROUND_HEIGHT, 
        256, 48, 
        0b00001000);
}
