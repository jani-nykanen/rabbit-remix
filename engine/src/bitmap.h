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

// Load a bitmap
Bitmap* load_bitmap(const char* path);

// Create a bitmap
Bitmap* create_bitmap(uint16 w, uint16 h, uint8* data);

// Destroy a bitmap
void destroy_bitmap(Bitmap* bmp);

#endif // __BITMAP__
