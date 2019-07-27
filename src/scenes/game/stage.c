#include "stage.h"

#include <math.h>

// Global bitmaps
static Bitmap* bmpFloor;
static Bitmap* bmpFence;
static Bitmap* bmpBush;
static Bitmap* bmpHouses;
static Bitmap* bmpClouds;
static Bitmap* bmpSky;


// Init global stage content
void init_global_stage(AssetManager* a) {

    // Get bitmaps
    bmpFloor = (Bitmap*)assets_get(a, "floor");
    bmpFence = (Bitmap*)assets_get(a, "fence");
    bmpBush = (Bitmap*)assets_get(a, "bush");
    bmpHouses = (Bitmap*)assets_get(a, "houses");
    bmpSky = (Bitmap*)assets_get(a, "sky");
    bmpClouds = (Bitmap*)assets_get(a, "clouds");
}


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
Stage create_stage() {

    Stage s;

    // Set initial values
    s.fencePos = 0.0f;
    s.housePos = 0.0f;
    s.fencePos = 0.0f;
    s.cloudPos = 0.0f;
    s.floorPos = 0.0f;

    return s;
}


// Update stage
void stage_update(Stage* s, float globalSpeed,  float tm) {

    const float BUSH_SPEED = 0.5f;
    const float HOUSE_SPEED = BUSH_SPEED / 2.0f;
    const float FENCE_SPEED = 1.0f;
    const float CLOUD_SPEED = 0.33f;
    const float FLOOR_SPEED = FENCE_SPEED;

    //
    // Update positions
    //
    update_scrolling_element(&s->fencePos, 
        FENCE_SPEED*globalSpeed, bmpFence->width, tm);
    update_scrolling_element(&s->bushPos, 
        BUSH_SPEED*globalSpeed, bmpBush->width, tm);
    update_scrolling_element(&s->housePos, 
        HOUSE_SPEED*globalSpeed, bmpHouses->width, tm);
    update_scrolling_element(&s->cloudPos, 
        CLOUD_SPEED*globalSpeed, bmpClouds->width, tm); 
    update_scrolling_element(&s->floorPos, 
        FLOOR_SPEED*globalSpeed, bmpFloor->width, tm);      
}


// Draw stage
void stage_draw(Stage* s, Graphics* g) {

    const int GROUND_HEIGHT = 32;
    const int BUSH_Y = 88;
    const int HOUSE_Y = 36;
    const int FENCE_Y = 192 - GROUND_HEIGHT - bmpFence->height;
    const int CLOUD_Y = 16;

    int i, p;

    // Sky
    g_draw_bitmap_fast(g, bmpSky, 0, 0);

    // Clouds
    draw_scrolling_element(g, bmpClouds, s->cloudPos, CLOUD_Y);
    // Houses
    draw_scrolling_element(g, bmpHouses, s->housePos, HOUSE_Y);
    // Bushes
    draw_scrolling_element(g, bmpBush, s->bushPos, BUSH_Y);
    // Fence
    draw_scrolling_element(g, bmpFence, s->fencePos, FENCE_Y);

    // Floor
    g_draw_3D_floor(g, bmpFloor, 0, 192-GROUND_HEIGHT, 
        256, GROUND_HEIGHT, -s->floorPos, 64, 1728);
}
