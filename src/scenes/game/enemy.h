//
// An enemy
// (c) 2019 Jani Nykänen
//

#ifndef __ENEMY__
#define __ENEMY__

#include <engine/sprite.h>
#include <engine/assets.h>

#include "coin.h"
#include "bullet.h"
#include "player.h"
#include "message.h"

// Initialize global enemy content
void init_global_enemies(AssetManager* a); 

// Enemy type
typedef struct {

    Vector2 pos;
    Vector2 startPos;
    Vector2 speed;

    Sprite spr;
    float radius;
    int id;

    Vector2 wave;
    int phase;

    float hurtTimer;
    float deathTimer;
    int health;
    bool exist;
    bool dying;
    bool stomped;

} Enemy;

// Create an enemy
Enemy create_enemy();

// Activate an enemy
void enemy_activate(Enemy* e, Vector2 pos, int id);

// Update an enemy
void enemy_update(Enemy* e, float globalSpeed, float tm);

// Bullet-enemy collision
void enemy_bullet_collision(
    Enemy* e, Bullet* b, Stats* s, 
    Coin* coins, int coinLen,
    Message* msgs, int msgLen);

// Player-enemy collision
void enemy_player_collision(Enemy* e, Player* pl,
    Coin* coins, int coinLen,
    Message* msgs, int msgLen);

// Draw an enemy
void enemy_draw(Enemy* e, Graphics* g);

#endif // __ENEMY__
