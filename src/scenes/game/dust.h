//
// "Dust" (not really)
// (c) 2019 Jani Nykänen
//

#ifndef __DUST__
#define __DUST__

#include <engine/graphics.h>


// Dust type
typedef struct 
{
    // Existence timer
    float timer;
    // Speed
    float speed;

    // Position
    Vector2 pos;
    // Source region in bitmap
    int sx, sy, sw, sh;

    // Does exist
    bool exist;

} Dust;

// Create dust
Dust create_dust();

// Activate
void dust_activate(Dust* d, Vector2 pos, float speed, 
    int sx, int sy, int sw, int sh);

// Update dust
void dust_update(Dust* d, float tm);

// Draw dust
void dust_draw(Dust* d, Graphics* g, Bitmap* bmp, uint8 col);

#endif // __DUST__
