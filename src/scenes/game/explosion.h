//
// KA-BOOM
// (c) 2019 Jani Nykänen
//

#ifndef __EXPLOSION__
#define __EXPLOSION__

#include <engine/graphics.h>

// Explosion type
typedef struct {

    Vector2 pos;
    float timer;
    float radius;

    bool exist;

} Explosion;

// Create an explosion
Explosion create_explosion();

// Activate an explosion
void exp_activate(Explosion* e, Vector2 pos, float radius);

// Update an explosion
void exp_update(Explosion* e, float tm);

// Draw an explosion
void exp_draw(Explosion* e, Graphics* g);

#endif // __EXPLOSION__
