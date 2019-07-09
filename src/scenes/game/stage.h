//
// Game stage
// (c) 2019 Jani Nykänen
//

#ifndef __STAGE__
#define __STAGE__

#include <engine/graphics.h>
#include <engine/assets.h>

// Stage type
typedef struct {

    // Bitmaps
    Bitmap* bmpFloor;
    Bitmap* bmpFence;
    Bitmap* bmpBush;
    Bitmap* bmpHouses;
    Bitmap* bmpClouds;
    Bitmap* bmpSky;

    // Positions
    float fencePos;
    float bushPos;
    float housePos;
    float cloudPos;
    float floorPos;

} Stage;

// Create stage
Stage create_stage(AssetManager* a);

// Update stage
void stage_update(Stage* s, float globalSpeed, float tm);

// Draw stage
void stage_draw(Stage* s, Graphics* g);

#endif // __STAGE__
