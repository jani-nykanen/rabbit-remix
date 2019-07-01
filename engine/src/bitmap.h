//
// Bitmap
// (c) 2019 Jani Nykänen
//


#ifndef __BITMAP__
#define __BITMAP__

#include "types.h"

#include <SDL2/SDL.h>

#include <stdbool.h>

// Alpha color
#define ALPHA 170

// Bitmap type
typedef struct {

    // Data
    uint8* data;

    // Dimensions
    uint16 width;
    uint16 height;

} Bitmap;

// Initialize bitmap loader
void init_bitmap_loader(SDL_Renderer* rend);

// Load a bitmap
Bitmap* load_bitmap(const char* path, bool dither);

// Create a bitmap
Bitmap* create_bitmap(uint16 w, uint16 h);

// Destroy a bitmap
void destroy_bitmap(Bitmap* bmp);

#endif // __BITMAP__
