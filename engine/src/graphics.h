//
// Graphics routines
// (c) 2019 Jani Nykänen
//


#ifndef __GRAPHICS__
#define __GRAPHICS__

#include <SDL2/SDL.h>

#include <stdbool.h>

#include "bitmap.h"
#include "config.h"


// Initialize global graphics content
int init_graphics_global();

// Destroy global graphics
void destroy_global_graphics();


// Colors
enum {

    ColorBlack = 0,
    ColorRed = 0b11100000,
    ColorWhite = 255,
};


// Pixel function types
enum {

    PixelFunctionDefault = 0,
    PixelFunctionDarken = 1,
    PixelFunctionSingleColor = 2,
    PixelFunctionSingleColorSkip = 3,
    PixelFunctionInverseFrame = 4,
    PixelFunctionInverse = 5,
    PixelFunctionInverseSkip = 6,
    PixelFunctionSkip = 7,
    PixelFunctionSkipSimple = 8,
    PixelFunctionSingleColorSkipSimple = 9,
    PixelFunctionLighten = 10,
};

// Graphics type
typedef struct {

    SDL_Window* window;
    SDL_Renderer* rend;

    // Canvas target size
    Point csize;
    // Window size
    Point windowSize;

    // Canvas info
    Point canvasPos;
    Point canvasScale;
    float aspectRatio;
    // Canvas bitmap
    SDL_Texture* canvas;
    // Canvas pixels
    uint8* pdata;
    // This buffer is used to copy the pixel data 
    // to it and rendering it for fancy effects
    uint8* pbuffer;
    // ...and as a bitmap format
    Bitmap bufferCopy;

    // Translation
    Point translation;

    // Darkness value
    int dvalue;
    // Darkness array
    uint8** darray;

    // Pixel function & param
    void (*pfunc) (void* g, int offset, uint8 col);
    int pparam1;
    int pparam2;

    // Things needed for textured triangles
    Mat2Fixed uvTransf;
    Point uvTrans;
    Vector2 uv1, uv2, uv3;
    Point top;
    Bitmap* tex;

} Graphics;

// Create a graphics component
Graphics* create_graphics(SDL_Window* window, Config* conf);

// Dispose graphics
void dispose_graphics(Graphics* g);

// Resize event
void g_resize(Graphics* g, int w, int h);

// Set pixel function
void g_set_pixel_function(Graphics* g, int func, int param1, int param2);

// Translate
void g_translate(Graphics* g, int tx, int ty);
// Move to
void g_move_to(Graphics* g, int dx, int dy);

// Pass data to the canvas
void g_update_pixel_data(Graphics* g);
// Refresh & draw the canvas
void g_refresh(Graphics* g);

// Clear screen
void g_clear_screen(Graphics* g, uint8 c);
// Draw some static
void g_draw_static(Graphics* g);

// Draw a bitmap
void g_draw_bitmap(Graphics* g, Bitmap* bmp, 
    int dx, int dy, bool flip);
// Draw a bitmap region
void g_draw_bitmap_region(Graphics* g, Bitmap* bmp, 
    int sx, int sy, int sw, int sh, 
    int dx, int dy, bool flip);
// Draw a scaled bitmap region
void g_draw_scaled_bitmap_region(Graphics* g, Bitmap* bmp, 
    int sx, int sy, int sw, int sh, 
    int dx, int dy, int dw, int dh, bool flip);

// Draw a bitmap fast
void g_draw_bitmap_fast(Graphics* g, Bitmap* bmp, 
    int dx, int dy);
// Draw a bitmap region fast
void g_draw_bitmap_region_fast(Graphics* g, Bitmap* bmp, 
    int sx, int sy, int sw, int sh, 
    int dx, int dy);

// Draw a waving bitmap
void g_draw_waving_bitmap(Graphics* g, Bitmap* bmp,
    int x, int y,
    float wave, int period, float amplitude);

// Draw scaled text
void g_draw_text(Graphics* g, Bitmap* bmp, const char* text,
    int dx, int dy, int xoff, int yoff,
    bool center);

// Fill a rectangle
void g_fill_rect(Graphics* g, int dx, int dy, 
    int dw, int dh, uint8 col);

// Draw a triangle
void g_draw_triangle(Graphics* g, 
    int x1, int y1, 
    int x2, int y2, 
    int x3, int y3, 
    uint8 col);

// Draw a line
void g_draw_line(Graphics* g, int x1, int y1, 
    int x2, int y2, uint8 col);

// Draw a thick line
void g_draw_thick_line(Graphics* g, 
    int dx1, int dy1, int dx2, int dy2, int r, uint8 col);

// Draw "3D" floor
void g_draw_3D_floor(Graphics* g, Bitmap* bmp,
    int x, int y, int w, int h, int xdelta, 
    int mx, int my);

// Set UV coordinates
void g_set_uv_coords(Graphics* g, 
    float u1, float v1, float u2, float v2, float u3, float v3);
// Enable texturing (pass NULL texture to disable)
void g_toggle_texturing(Graphics* g, Bitmap* tex);

// Darken the screen
void g_darken(Graphics* g, int level);

// Set darkness tint color
// (I know right)
void g_set_darkness_color(Graphics* g, uint8 col);

// Copy current screen to the buffer
void g_copy_to_buffer(Graphics* g);

// Fill the screen with zoomed rotated bitmap
void g_fill_zoomed_rotated(Graphics* g, Bitmap* bmp, 
    float angle, float sx, float sy);

// Fill the screen with a color outside a circle area.
// You know what I mean
void g_fill_circle_outside(Graphics* g, int radius, uint8 col);

#endif // __GRAPHICS__
