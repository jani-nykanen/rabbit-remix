#include "sprite.h"


// Create a sprite
Sprite create_sprite(int w, int h) {

    Sprite s;
    s.width = w;
    s.height = h;
    s.frame = 0;
    s.row = 0;
    s.count = 0.0f;
    
    return s;
}


// Animate
void spr_animate(Sprite* s, 
    int row, int start, int end, 
    float speed, float tm) {

    // If starting & ending frames are the same
	if (start == end) {

        s->count = 0;
        s->frame= start;
        s->row = row;
        return;
    }

    // Set to the current frame and start
    // animation from the beginning
    if (s->row != row) {

        s->count = 0;
        s->frame= end > start ? start : end;
        s->row = row;
    }

    if (start < end && s->frame< start) {            
                
        s->frame= start;
    }
    else if (end < start && s->frame< end) {

        s->frame= end;
    }

    // Animate
    s->count += 1.0f * tm;
    if (s->count > speed) {

        if (start < end) {

            if (++s->frame> end) {

                s->frame= start;
            }
        }
        else {

            if (--s->frame< end) {

                s->frame= start;
            }
        }

        s->count -= speed;
    }
}


// Draw frame
void spr_draw_frame(Sprite* s, Graphics* g, Bitmap* bmp, 
    int x, int y, int frame, int row, bool flip) {

    g_draw_bitmap_region(g, bmp, 
        s->frame*s->width, s->row*s->height, 
        s->width, s->height, x, y, flip);
}


// Draw
void spr_draw(Sprite* s, Graphics* g, Bitmap* bmp, 
    int x, int y, bool flip) {

    spr_draw_frame(s, g, bmp, x, y, s->frame, s->row, flip);
}
