//
// Spikeball
// (c) 2019 Jani Nykänen
//

#ifndef __SPIKEBALL__
#define __SPIKEBALL__

#include <engine/graphics.h>
#include <engine/assets.h>

#include "player.h"
#include "bullet.h"

// Initialize global content
void init_global_spikeballs(AssetManager* a);

// Spikeball type
typedef struct {

    Vector2 pos;
    Vector2 startPos;
    Vector2 speed;
    float maxY;
    float maxDist;
    float startDist;
    int type;
    bool exist;

    float wave;

} Spikeball;

// Create a spikeball
Spikeball create_spikeball();

// Activate a spikeball
void sb_activate(Spikeball* b, float x, float maxY, int type);

// Update a spikeball
void sb_update(Spikeball* b, float globalSpeed, float tm);

// Spikeball-player collision
void sb_player_collision(Spikeball* b, Player* pl);

// Spikeball-bullet collision
void sb_bullet_collision(Spikeball* sb, Bullet* b);

// Draw the spikeball shadow
void sb_draw_shadow(Spikeball* b, Graphics* g) ;

// Draw a spikeball
void sb_draw(Spikeball* b, Graphics* g);

#endif // __SPIKEBALL__
