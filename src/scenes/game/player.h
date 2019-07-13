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

#include "dust.h"
#include "bullet.h"

// Maximum dust count
#define DUST_COUNT 16
// Maximum bullet count
#define BULLET_COUNT 16

// Init global data
void init_global_player(AssetManager* a);

// Player type
typedef struct {

    // Position & speed
    Vector2 pos;
    Vector2 oldPos;
    Vector2 speed;
    Vector2 target;

    // Jump variables
    float jumpTimer;
    float jumpSpeedMul;
    bool extendJump;
    bool doubleJump;
    bool djumpReleased;
    bool flapping;
    // Falling
    bool quickFall;

    // Sprite
    Sprite spr;

    // Dust
    Dust dust [DUST_COUNT];
    float dustTimer;

    // Bullets
    Bullet bullets [BULLET_COUNT];
    float shootWait;
    float blastTime;
    // "Loading"
    bool loading;
    float loadTimer;

} Player;

// Create a player
Player create_player(int x, int y);

// Update player
void pl_update(Player* pl, EventManager* evMan, float tm);

// Draw the player shadow
void pl_draw_shadow(Player* pl, Graphics* g);

// Draw player
void pl_draw(Player* pl, Graphics* g);

// Jump collision
bool pl_jump_collision(Player* pl, float x, float y, float w, float power);

#endif // __PLAYER__
