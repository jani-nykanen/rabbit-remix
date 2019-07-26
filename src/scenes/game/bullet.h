//
// A bullet
// (c) 2019 Jani Nykänen
//

#ifndef __BULLET__
#define __BULLET__

#include <engine/graphics.h>

// Bullet type
typedef struct 
{
    Vector2 pos;
    Vector2 speed;

    bool exist;
    bool dying;
    bool isSpecial;

    float angle;
    float radius;
    float deathTimer;

} Bullet;

// Create bullet
Bullet create_bullet();

// Activate a bullet
void bullet_activate(Bullet* b, Vector2 pos, Vector2 speed, 
    float radius, bool spc);

// Update a bullet
void bullet_update(Bullet* b, float tm);

// Draw a bullet
void bullet_draw(Bullet* b, Graphics* g);

// Kill a bullet
void bullet_kill(Bullet* b);

#endif // __BULLET__
