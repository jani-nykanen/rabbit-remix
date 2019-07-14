//
// Mushrooms
// (c) 2019 Jani Nykänen
//

#ifndef __MUSHROOM__
#define __MUSHROOM__

#include <engine/graphics.h>
#include <engine/assets.h>
#include <engine/sprite.h>

#include "player.h"

// Initialize global content
void init_global_mushrooms(AssetManager* a);

// Mushroom type
typedef struct {

    // Position
    Vector2 pos;
    Vector2 startPos;
    // Major type (=row)
    int majorType;
    // Minor type (=column)
    int minorType;

    // "Bounce" timer & stuff
    float bounceTimer;

    // Jumping (for mushrooms that jump)
    float jumpTimer;
    float gravity;

    // Flying
    float wave;
    float middlePos;

    // Sprite
    Sprite spr;

    // Does exist
    bool exist;

} Mushroom;

// Create a mushroom
Mushroom create_mushroom();

// Activate a mushroom
float mush_activate(Mushroom* m, Vector2 pos, int major, int minor);

// Update a mushroom
void mush_update(Mushroom* m, float globalSpeed, float tm);

// Player collision
void mush_player_collision(Mushroom* m, Player* pl);

// Draw a mushroom
void mush_draw(Mushroom* m, Graphics* g);

#endif // __MUSHROOM__
