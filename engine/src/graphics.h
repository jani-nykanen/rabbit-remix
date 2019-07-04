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
#include "trianglebuffer.h"
#include "matrix.h"

// Matrix stack size
#define MATRIX_STACK_SIZE 32 

// Flipping flags
enum {

    FlipNone = 0,
    FlipHorizontal = 1,
    FlipVertical = 2,
    FlipBoth = FlipHorizontal | FlipVertical // == 3
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

    // Translation
    Point translation;

    // Matrices
    Matrix4 model;
    Matrix4 view;
    Matrix4 projection;
    Matrix4 product;
    // Is the product computed
    bool productComputed;

    // Triangle buffer
    TriangleBuffer tbuf;

    // Matrix stack for model space
    Matrix4 modelStack [MATRIX_STACK_SIZE];
    int stackPointer;

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
    int dx, int dy, bool flip);
// Draw a bitmap region
void g_draw_bitmap_region(Graphics* g, Bitmap* bmp, 
    int sx, int sy, int sw, int sh, 
    int dx, int dy, bool flip);
// Draw a scaled bitmap region
void g_draw_scaled_bitmap_region(Graphics* g, Bitmap* bmp, 
    int sx, int sy, int sw, int sh, 
    int dx, int dy, bool flip);

// Draw a bitmap fast
void g_draw_bitmap_fast(Graphics* g, Bitmap* bmp, 
    int dx, int dy);
// Draw a bitmap region fast
void g_draw_bitmap_region_fast(Graphics* g, Bitmap* bmp, 
    int sx, int sy, int sw, int sh, 
    int dx, int dy);
// Draw a scaled bitmap region fast
void g_draw_scaled_bitmap_region_fast(Graphics* g, Bitmap* bmp, 
    int sx, int sy, int sw, int sh, 
    int dx, int dy);

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

// Draw a 3D triangle
void g_draw_triangle_3D(Graphics* g,
    Vector3 A, Vector3 B, Vector3 C,
    uint8 col);

// Draw triangle buffer
void g_draw_triangle_buffer(Graphics* g);

// Draw a line
void g_draw_line(Graphics* g, int x1, int y1, 
    int x2, int y2, uint8 col);


  // ---------------- //
 // Transformations  //
// ---------------- //

// Set model matrix to identity matrix
void g_load_identity(Graphics* g);

// Translate model space
void g_translate_model(Graphics* g, float x, float y, float z);

// Scale model space
void g_scale_model(Graphics* g, float x, float y, float z);

// Rotate model space
void g_rotate_model(Graphics* g, 
    float angle, float x, float y, float z);

// Set perspective matrix
void g_set_perspective(Graphics* g, 
    float fovY, float near, float far);

// Push the current model transformation to the
// stack
void g_push(Graphics* g);

// Pop the model transformation from the stack
void g_pop(Graphics* g);

#endif // __GRAPHICS__
