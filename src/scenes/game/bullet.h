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

    float angle;
    float radius;

} Bullet;

// Create bullet
Bullet create_bullet();

// Activate a bullet
void bullet_activate(Bullet* b, Vector2 pos, Vector2 speed, float radius);

// Update a bullet
void bullet_update(Bullet* b, float tm);

// Draw a bullet
void bullet_draw(Bullet* b, Graphics* g);


#endif // __BULLET__
