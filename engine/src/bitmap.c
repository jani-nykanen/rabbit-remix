#include "bitmap.h"

#include "err.h"

#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#include "lib/stb_image.h"

// A reference to the renderer
static SDL_Renderer* rendRef =NULL;



// Initialize bitmap loader
void init_bitmap_loader(SDL_Renderer* rend) {

    rendRef = rend;
}


// Load a bitmap
Bitmap* load_bitmap(const char* path, bool dither) {

    const float DIVISOR = 36.428f;
    const int DIVISOR2 = 85;

    // Allocate memory
    Bitmap* bmp = (Bitmap*)malloc(sizeof(Bitmap));
    if (bmp == NULL) {

        ERR_MEM_ALLOC;
        return NULL;
    }

    // Load the image
    int comp, w, h;
    uint8* pdata = stbi_load(path, &w, &h, &comp, STBI_rgb_alpha);
    if (pdata == NULL) {

        err_throw_param_1("Failed to load a bitmap in ", path);
        return NULL;
    }
    // Store dimensions
    bmp->width = w;
    bmp->height = h;

    // Allocate memory
    bmp->data = (uint8*)malloc(sizeof(uint8) * w * h);
    if (bmp->data == NULL) {

        ERR_MEM_ALLOC;
        free(bmp);
        return NULL;
    }

    // Convert to a proper format
    int i = 0;
    int row = 0;
    int column = 0;
    uint8 pixel;
    uint8 r,g,b,a;
    uint8 er,eg,eb;
    uint8 p = STBI_rgb_alpha;
    for(; i < w*h; i++) {

        row = i / w;
        column = i % w;

        // Check alpha
        if (p == 4) {

            a = pdata[i*4 +3];
            if (a < 255) {
                
                bmp->data[i] = ALPHA;
                continue;
            }
        }

        b = pdata[i*p +2];
        g = pdata[i*p +1];
        r = pdata[i*p ];

        // No dithering
        if (!dither) {

            er = (uint8) round((float)r / DIVISOR);
            if (er > 7) r = 7;
            er = er << 5;
            eg = (uint8) round((float)g / DIVISOR);
            if (eg > 7) eg = 7;
            eg = eg << 2;
            eb = (b / DIVISOR2);
        }
        // Dithering
        else {

            float (*func) (float);

            // Even
            if (row % 2 == column % 2) {

                func = floorf;
            }
            // Odd
            else {

                func = ceilf;
            }

            er = (Uint8) func(r / (DIVISOR/2.0f) );
            eg = (Uint8) func(g / (DIVISOR/2.0f) );
            eb = (Uint8) func(b / ((float)DIVISOR2 / 2.0f));

            er = er / 2;
            eg = eg / 2;
            eb = eb / 2;

            // Limit
            if (er > 7) er = 7;
            if (eg > 7) eg = 7;
            if (eb > 3) eb = 3;

            er = er << 5;
            eg = eg << 2;
        }

        pixel = er | eg | eb;

        bmp->data[i] = pixel;
    }

    return bmp;
}


// Create a bitmap
Bitmap* create_bitmap(uint16 w, uint16 h) {

    // Allocate memory
    Bitmap* bmp = (Bitmap*)malloc(sizeof(Bitmap));
    if (bmp == NULL) {

        ERR_MEM_ALLOC;
        return NULL;
    }

    // Set dimensions
    bmp->width = w;
    bmp->height = h;

    // Allocate memory for data
    bmp->data = (uint8*)malloc(sizeof(uint8) * w * h);
    if (bmp->data == NULL) {

        ERR_MEM_ALLOC;
        free(bmp);
        return NULL;
    }

    return bmp;
}


// Destroy a bitmap
void destroy_bitmap(Bitmap* bmp) {

    if (bmp == NULL) return;

    free(bmp->data);
    free(bmp);
}
