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

// Flipping flags
enum {

    FlipNone = 0,
    FlipHorizontal = 1,
    FlipVertical = 2,
    FlipBoth = FlipHorizontal | FlipVertical // == 3
};

// Color
typedef struct {

    uint8 r,g,b,a;
} Color;

// Color constructors
Color rgb(uint8 r, uint8 g, uint8 b);
Color rgba(uint8 r, uint8 g, uint8 b, uint8 a);


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
    // Canvas bitmap
    SDL_Texture* canvas;
    // Canvas pixels
    uint8* pdata;

    // Translation
    Point translation;

    // Global color
    Color blendColor;

} Graphics;

// Create a graphics component
Graphics* create_graphics(SDL_Window* window, Config* conf);

// Dispose graphics
void dispose_graphics(Graphics* g);

// Resize event
void g_resize(Graphics* g, int w, int h);

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
    int dx, int dy, int flip);
// Draw a scaled bitmap
void g_draw_scaled_bitmap(Graphics* g, Bitmap* bmp, 
   int dx, int dy, int dw, int dh, int flip);
// Draw a bitmap region
void g_draw_bitmap_region(Graphics* g, Bitmap* bmp, 
    int sx, int sy, int sw, int sh, 
    int dx, int dy, int flip);
// Draw a scaled bitmap region
void g_draw_scaled_bitmap_region(Graphics* g, Bitmap* bmp, 
    int sx, int sy, int sw, int sh, 
    int dx, int dy, int dw, int dh,
    int flip);

// Draw scaled text
void g_draw_text(Graphics* g, Bitmap* bmp, const char* text,
    int dx, int dy, int xoff, int yoff,
    bool center);

// Fill a rectangle
void g_fill_rect(Graphics* g, int dx, int dy, 
    int dw, int dh, uint8 col);

#endif // __GRAPHICS__
