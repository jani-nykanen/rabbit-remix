//
// Body
// (c) 2019 Jani Nykänen
//

#ifndef __BODY__
#define __BODY__

#include <engine/graphics.h>
#include <engine/sprite.h>

// Body type (heh)
typedef struct {

    Vector2 pos;
    Sprite spr;

    bool exist;

} Body;

// Create a body
Body create_body();

// Activate body
void body_activate(Body* b, Vector2 pos, Sprite spr);

// Update body
void body_update(Body* b, float globalSpeed, float tm);

// Draw a body
void body_draw(Body* b, Graphics* g, Bitmap* bmp);

#endif // __BODY__
