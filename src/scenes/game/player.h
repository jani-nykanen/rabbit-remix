//
// Player object
// (c) 2019 Jani Nykänen
//

#ifndef __PLAYER__
#define __PLAYER__

#include <engine/types.h>
#include <engine/sprite.h>
#include <engine/eventmanager.h>
#include <engine/assets.h>

// Init global data
void init_global_player(AssetManager* a);

// Player type
typedef struct {

    // Position & speed
    Vector2 pos;
    Vector2 speed;
    Vector2 target;

    // Jump variables
    float jumpTimer;
    bool doubleJump;

    // Sprite
    Sprite spr;

} Player;

// Create a player
Player create_player(int x, int y);

// Update player
void pl_update(Player* pl, EventManager* evMan, float tm);

// Draw player
void pl_draw(Player* pl, Graphics* g);

#endif // __PLAYER__
