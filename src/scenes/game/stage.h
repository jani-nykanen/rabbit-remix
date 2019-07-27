//
// Game stage
// (c) 2019 Jani Nykänen
//

#ifndef __STAGE__
#define __STAGE__

#include <engine/graphics.h>
#include <engine/assets.h>

// Sorry
#define GROUND_COLLISION_HEIGHT 21

// Init global stage content
void init_global_stage(AssetManager* a);

// Stage type
typedef struct {

    // Positions
    float fencePos;
    float bushPos;
    float housePos;
    float cloudPos;
    float floorPos;

} Stage;

// Create stage
Stage create_stage();

// Update stage
void stage_update(Stage* s, float globalSpeed, float tm);

// Draw stage
void stage_draw(Stage* s, Graphics* g);

#endif // __STAGE__
