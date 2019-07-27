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
#include "body.h"
#include "stats.h"
#include "explosion.h"
#include "message.h"

// Maximum dust count
#define DUST_COUNT 16
// Maximum bullet count
#define BULLET_COUNT 16
// Maximum body count (ehhehehe)
#define BODY_COUNT 4

// Init global data
void init_global_player(AssetManager* a);

// Player type
typedef struct {

    // Position & speed
    Vector2 pos;
    Vector2 oldPos;
    Vector2 startPos;
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

    // Bodies
    Body bodies [BODY_COUNT];

    // Arrow "wave" effect
    float arrowWave;

    // If dying
    bool dying;
    // Respawn timer
    float respawnTimer;
    // Gotta love this word
    float invincibilityTimer;
    // Has jumped the first jump
    bool firstJump;

    // Self-destruct timer ('cause why not)
    float selfDestructTimer;
    // Explosion
    Explosion exp;

    // Stats
    Stats* stats;

    // Game over event
    void (*eventGameOver) (EventManager* evMan);
    
} Player;

// Create a player
Player create_player(int x, int y, Stats* stats,
    void (*ev)(EventManager* evMan));

// Update player
void pl_update(Player* pl, EventManager* evMan, 
    float globalSpeed, float tm,
    void* coins, int coinLen,
    Message* msgs, int msgLen);

// Shake
void pl_shake(Player* pl, Graphics* g);

// Draw the player shadow
void pl_draw_shadow(Player* pl, Graphics* g);

// Draw entrance portal
void pl_draw_entrance_portal(Player* pl, Graphics* g);

// Draw player
void pl_draw(Player* pl, Graphics* g);

// Draw player explosion
void pl_draw_explosion(Player* pl, Graphics* g);

// Jump collision
bool pl_jump_collision(Player* pl, float x, float y, float w, float power);

// Kill a player
void pl_kill(Player* pl, int type);

#endif // __PLAYER__
