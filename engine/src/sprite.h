//
// Animated sprite
// (c) 2019 Jani Nykänen
//

#ifndef __SPRITE__
#define __SPRITE__

#include "graphics.h"

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

// Animate sprite
void spr_animate(Sprite* s, 
    int row, int start, int end, 
    float speed, float tm);

// Draw sprite frame
void spr_draw_frame(Sprite* s, Graphics* g, Bitmap* bmp, 
    int x, int y, int frame, int row, bool flip);

// Draw sprite
void spr_draw(Sprite* s, Graphics* g, Bitmap* bmp, 
    int x, int y, bool flip);

// Draw scaled sprite frame
void spr_draw_scaled_frame(Sprite* s, Graphics* g, Bitmap* bmp, 
    int x, int y, int frame, int row, int sx, int sy, bool flip);

// Draw scaled sprite
void spr_draw_scaled(Sprite* s, Graphics* g, Bitmap* bmp, 
    int x, int y, int sx, int sy, bool flip);


#endif // __SPRITE__
