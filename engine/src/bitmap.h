//
// Bitmap
// (c) 2019 Jani Nykänen
//


#ifndef __BITMAP__
#define __BITMAP__

#include "types.h"

#include <SDL2/SDL.h>

#include <stdbool.h>

// Bitmap type
typedef struct {

    // SDL2 texture
    SDL_Texture* tex;

    // Dimensions
    uint16 width;
    uint16 height;

} Bitmap;

// Initialize bitmap loader
void init_bitmap_loader(SDL_Renderer* rend);

// Load a bitmap
Bitmap* load_bitmap(const char* path, bool linearFiltering);

// Create a bitmap
Bitmap* create_bitmap(uint16 width, uint16 height, bool target);

// Destroy a bitmap
void destroy_bitmap(Bitmap* bmp);

#endif // __BITMAP__
