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
Bitmap* load_bitmap(const char* path, bool linearFiltering) {

    Uint32 rmask, gmask, bmask, amask;
    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
    #else // Little endian
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
    #endif

    // Allocate memory
    Bitmap* bmp = (Bitmap*)malloc(sizeof(Bitmap));
    if(bmp == NULL) {

        ERR_MEM_ALLOC;
        return NULL;
    }

    // Load the image
    int comp, w, h;
    Uint8* pdata = stbi_load(path, &w, &h, &comp, STBI_rgb_alpha);
    if(pdata == NULL) {

        err_throw_param_1("Failed to load a bitmap in ", path);
        return NULL;
    }
    // Store dimensions
    bmp->width = w;
    bmp->height = h;

    // Convert to an SDL2 surface
    SDL_Surface* surf = SDL_CreateRGBSurfaceFrom(
        (void*)pdata, w, h, 32, 4*w,
            rmask, gmask, bmask, amask);
    if(surf == NULL) {

        err_throw_no_param("Failed to create a surface.");
    }

    // Set hints
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, 
        linearFiltering ? "1" : "0");

    // Use surface to create a texture
    bmp->tex = SDL_CreateTextureFromSurface(rendRef, surf);
    if(bmp->tex == NULL) {

        err_throw_no_param("Failed to create a texture.");
    }
    // Set blend mode
    SDL_SetTextureBlendMode(bmp->tex,
        SDL_BLENDMODE_BLEND);

    // Dispose the surface
    SDL_FreeSurface(surf);

    // Initialize properites
    bmp->center = point(0, 0);
    bmp->angle = 0.0f;

    return bmp;
}


// Destroy a bitmap
void destroy_bitmap(Bitmap* bmp) {

    if(bmp == NULL) return;

    SDL_DestroyTexture(bmp->tex);
    free(bmp);
}
