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

    // Viewport target size
    Point viewTarget;
    // Logical viewport size
    Point viewSize;
    // Viewport top-left corner
    Point viewTopLeft;
    // Viewport rect
    SDL_Rect viewRect;
    // Window size
    Point windowSize;

    // Translation & scale
    Point translation;
    float scale;
    float defaultScale;

    // Global color
    Color blendColor;

} Graphics;

// Create a graphics component
Graphics* create_graphics(SDL_Window* window, Config* conf);

// Dispose graphics
void dispose_graphics(Graphics* g);

// Set global color
void g_set_blend_color(Graphics* g, Color c);
// Reset blend color
void g_reset_blend_color(Graphics* g);

// Resize event
void g_resize(Graphics* g, int w, int h);

// Use viewport
void g_use_viewport(Graphics* g);
// Reset viewport
void g_reset_viewport(Graphics* g);

// Reset view
void g_reset_view(Graphics* g);
// Center view
void g_center_view(Graphics* g);
// Translate
void g_translate(Graphics* g, int tx, int ty);
// Move to
void g_move_to(Graphics* g, int dx, int dy);

// Refresh
void g_refresh(Graphics* g);

// Clear background
void g_clear_background(Graphics* gr, uint8 r, uint8 g, uint8 b);

// Clear screen
void g_clear_screen(Graphics* gr, uint8 r, uint8 g, uint8 b);
void g_clear_screen_rgba(Graphics* gr, uint8 r, uint8 g, uint8 b, uint8 a);

// Draw a bitmap
void g_draw_bitmap(Graphics* g, Bitmap* bmp, 
    float dx, float dy, int flip);
// Draw a scaled bitmap
void g_draw_scaled_bitmap(Graphics* g, Bitmap* bmp, 
   float dx, float dy, float dw, float dh, int flip);
// Draw a bitmap region
void g_draw_bitmap_region(Graphics* g, Bitmap* bmp, 
    int sx, int sy, int sw, int sh, 
    float dx, float dy, int flip);
// Draw a scaled bitmap region
void g_draw_scaled_bitmap_region(Graphics* g, Bitmap* bmp, 
    int sx, int sy, int sw, int sh, 
    float dx, float dy, float dw, float dh,
    int flip);

// Draw scaled text
void g_draw_scaled_text(Graphics* g, Bitmap* bmp, const char* text,
    float dx, float dy, float xoff, float yoff, float scalex, float scaley, 
    bool center);

// Fill a rectangle
void g_fill_rect(Graphics* g, float dx, float dy, 
    float dw, float dh, Color c);

#endif // __GRAPHICS__
