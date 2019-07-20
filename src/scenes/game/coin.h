//
// Coin
// (c) 2019 Jani Nykänen
//

#ifndef __COIN__
#define __COIN__

#include <engine/sprite.h>
#include <engine/assets.h>

#include "player.h"

// Init global
void init_global_coins(AssetManager* a);

// Coin type
typedef struct {

    Vector2 pos;
    Vector2 speed;

    Sprite spr;
    bool exist;

    // Could be "disappearing" as well 
    bool dying;
    float deathTimer;

    float wait;

} Coin;

// Create a coin
Coin create_coin();

// Activate a coin
void coin_activate(Coin* c, Vector2 pos, Vector2 speed);

// Update a coin
void coin_update(Coin* c, float globalSpeed, float tm);

// Coin-player collision
void coin_player_collision(Coin* c, Player* pl);

// Draw a coin
void coin_draw(Coin* c, Graphics* g);

// Get the next available coin in the array
Coin* coin_get_next(Coin* coins, int len);

#endif // __COIN__
