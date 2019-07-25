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
    int magnitude;

    bool exist;
    bool dead;

} Explosion;

// Create an explosion
Explosion create_explosion();

// Activate an explosion
void exp_activate(Explosion* e, Vector2 pos, int magnitude);

// Update an explosion
void exp_update(Explosion* e, float tm);

// Draw an explosion
void exp_draw(Explosion* e, Graphics* g);

// Shake
void exp_shake(Explosion* e,  Graphics* g);

// Get radius
float exp_get_radius(Explosion* e);

// Is dead
bool exp_dead(Explosion* e);

#endif // __EXPLOSION__
