//
// Animated sprite
// (c) 2019 Jani Nykänen
//

#ifndef __SPRITE__
#define __SPRITE__

#include <engine/graphics.h>

// Sprite type
typedef struct  
{
    int width;
    int height;
    int frame;
    int row;
    float count;

} Sprite;

// Create a sprite
Sprite create_sprite(int w, int h);

// Animate
void spr_animate(Sprite* s, 
    int row, int start, int end, 
    float speed, float tm);

// Draw frame
void spr_draw_frame(Sprite* s, Graphics* g, Bitmap* bmp, 
    int x, int y, int frame, int row, bool flip);

// Draw
void spr_draw(Sprite* s, Graphics* g, Bitmap* bmp, 
    int x, int y, bool flip);


#endif // __SPRITE__
