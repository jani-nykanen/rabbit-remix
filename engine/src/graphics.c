#include "graphics.h"

#include "bitmap.h"
#include "err.h"


// Color constructors
Color rgb(uint8 r, uint8 g, uint8 b) {

    return rgba(r, g, b, 255);
}
Color rgba(uint8 r, uint8 g, uint8 b, uint8 a) {

    return (Color){r,g,b, a};
}


// Apply transformation
static void apply_transform(Graphics *g, 
    float* dx, float* dy, float* dw, float* dh) {

    // Apply translation
    *dx += g->translation.x;
    *dy += g->translation.y;

    // Apply scale
    *dx = (*dx)*g->scale;
    *dy = (*dy)*g->scale;
    *dw = (*dw)*g->scale;
    *dh = (*dh)*g->scale;
}


// Create a graphics component
Graphics* create_graphics(SDL_Window* window, Config* conf) {

    // Allocate memory
    Graphics* g = (Graphics*)malloc(sizeof(Graphics));
    if(g == NULL) {

        ERR_MEM_ALLOC;
        return NULL;
    }

    // Store window, just for sure
    g->window = window;

    // Create a renderer
    g->rend = SDL_CreateRenderer(window, -1,
            SDL_RENDERER_ACCELERATED  |
            SDL_RENDERER_PRESENTVSYNC |
            SDL_RENDERER_TARGETTEXTURE);
    if(g->rend == NULL) {

        err_throw_no_param("Failed to create a renderer.");
        return NULL;
    }

    // Pass renderer to the bitmap loader
    init_bitmap_loader(g->rend);

    // Read view target size
    g->viewTarget.x = conf_get_param_int(conf, "viewport_width", 1280);
    g->viewTarget.y = conf_get_param_int(conf, "viewport_height", 720);

    // Set defaults
    g->viewSize = g->viewTarget;
    g->viewTopLeft = point(0, 0);

    // Resize
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    g_resize(g, w, h);

    // Enable blending for primitives
    SDL_SetRenderDrawBlendMode(g->rend, SDL_BLENDMODE_BLEND);

    return g;
}


// Dispose graphics
void dispose_graphics(Graphics* g) {

    if(g == NULL) return;

    SDL_DestroyRenderer(g->rend);
    free(g);
}


// Set global color
void g_set_blend_color(Graphics* g, Color c) {

    g->blendColor = c;
}


// Reset blend color
void g_reset_blend_color(Graphics* g) {

    g_set_blend_color(g, rgb(255, 255, 255));
}


// Resize event
void g_resize(Graphics* g, int w, int h) {

    float newAspect = (float)w / (float)h;
    float targetAspect = (float)g->viewTarget.x / (float)g->viewTarget.y;

    // If the same aspect ratio or wider
    if(newAspect >= targetAspect) {

        g->viewSize.x = (int)((float)h*targetAspect);
        g->viewSize.y = h;

        g->viewTopLeft.x = w/2 - g->viewSize.x/2;
        g->viewTopLeft.y = 0;

        g->defaultScale = (float)h / (float)g->viewTarget.y;
    }
    else {

        g->viewSize.x = w;
        g->viewSize.y = (int)(((float)w)/targetAspect);

        g->viewTopLeft.x = 0;
        g->viewTopLeft.y = h/2 - g->viewSize.y/2;

        g->defaultScale = (float)w / (float)g->viewTarget.x;
    }
    
    // Store view
    g->viewRect = (SDL_Rect){
        g->viewTopLeft.x, g->viewTopLeft.y, 
        g->viewSize.x, g->viewSize.y
    };
    // Store window size
    g->windowSize = point(w, h);
}


// Use viewport
void g_use_viewport(Graphics* g) {
    
    // Set viewport & scale
    SDL_RenderSetViewport(g->rend, &g->viewRect);   

    g->scale = g->defaultScale;
    // SDL_RenderSetScale(g->rend, g->scale, g->scale);
}


// Reset viewport
void g_reset_viewport(Graphics* g) {

    SDL_RenderSetViewport(g->rend, NULL);

    // Reset scale & translation
    g->scale = 1;
    g->translation = point(0, 0);
}


// Reset view
void g_reset_view(Graphics* g) {

    g_move_to(g, 0, 0);
    g->scale = g->defaultScale;
}


// Center view
void g_center_view(Graphics* g) {

    g_move_to(g, g->viewTarget.x/2, g->viewTarget.y/2);
}


// Translate
void g_translate(Graphics* g, int tx, int ty) {

    g->translation.x += tx;
    g->translation.y += ty;
}


// Move to
void g_move_to(Graphics* g, int dx, int dy) {

    g->translation.x = dx;
    g->translation.y = dy;
}


// Refresh
void g_refresh(Graphics* g) {

    SDL_RenderPresent(g->rend);
}


// Clear background
void g_clear_background(Graphics* gr, uint8 r, uint8 g, uint8 b) {

    SDL_SetRenderDrawColor(gr->rend, r, g, b, 255);
    SDL_RenderClear(gr->rend);
}


// Clear screen
void g_clear_screen(Graphics* gr, uint8 r, uint8 g, uint8 b) {

    g_clear_screen_rgba(gr, r, g, b, 255);
}
void g_clear_screen_rgba(Graphics* gr, uint8 r, uint8 g, uint8 b, uint8 a) {

    // Set color
    SDL_SetRenderDrawColor(gr->rend, r, g, b, a);
    // Draw a filled rectangle
    SDL_Rect dest = (SDL_Rect) {
        0, 0, gr->viewSize.x, gr->viewSize.y
    };
    SDL_RenderFillRect(gr->rend, &dest);
}


// Draw a bitmap
void g_draw_bitmap(Graphics* g, Bitmap* bmp, 
    float dx, float dy, int flip) {

    g_draw_scaled_bitmap_region(g, bmp,
        0, 0, bmp->width, bmp->height, 
        dx, dy, bmp->width, bmp->height, 
        flip);
}


// Draw a scaled bitmap
void g_draw_scaled_bitmap(Graphics* g, Bitmap* bmp, 
   float dx, float dy, float dw, float dh, int flip) {

    g_draw_scaled_bitmap_region(g, bmp,
        0, 0, bmp->width, bmp->height, 
        dx, dy, dw, dh, flip);
}


// Draw a bitmap region
void g_draw_bitmap_region(Graphics* g, Bitmap* bmp, 
    int sx, int sy, int sw, int sh, 
    float dx, float dy, int flip) {

    g_draw_scaled_bitmap_region(g, bmp, sx, sy, sw, sh,
        dx, dy, sw, sh, flip);
}


// Draw a scaled bitmap region
void g_draw_scaled_bitmap_region(Graphics* g, Bitmap* bmp, 
    int sx, int sy, int sw, int sh, 
    float dx, float dy, float dw, float dh,
    int flip) {

    // Apply blend color
    SDL_SetTextureColorMod(bmp->tex, 
        g->blendColor.r, g->blendColor.g, g->blendColor.b);
    SDL_SetTextureAlphaMod(bmp->tex, g->blendColor.a);

    // Set transform
    apply_transform(g, &dx, &dy, &dw, &dh);
    dx -= bmp->center.x*g->scale;
    dy -= bmp->center.y*g->scale;

    // Set source & destination
    SDL_Rect src = (SDL_Rect){sx, sy, sw, sh};
    SDL_Rect dst = (SDL_Rect){(int)dx, (int)dy, (int)dw, (int)dh};
    SDL_Point p = (SDL_Point){
        (int)bmp->center.x*g->scale, 
        (int)bmp->center.y*g->scale
    };

    // Draw
    SDL_RenderCopyEx(g->rend, bmp->tex, &src, &dst,
            bmp->angle, &p, (SDL_RendererFlip)flip);
}


// Draw scaled text
void g_draw_scaled_text(Graphics* g, Bitmap* bmp, const char* text,
    float dx, float dy, float xoff, float yoff, float scalex, float scaley, 
    bool center) {

    int cw = (bmp->width) / 16;
    int ch = cw;
    int len = strlen(text);

    float x = dx;
    float y = dy;
    unsigned char c;

    // Center the text
    if (center)
    {
        dx -= ((len + 1) / 2.0f * (cw + xoff) * scalex);
        x = dx;
    }

    // Draw every character
    int sx, sy;
    for (int i = 0; i < len; ++i)
    {

        c = text[i];
        // Line swap
        if (c == '\n')
        {
            x = dx;
            y += (yoff + ch) * scaley;
            continue;
        }

        sx = c % 16;
        sy = (c / 16);
        // Draw character
        g_draw_scaled_bitmap_region(g, 
            bmp, sx * cw, sy * ch, cw, ch,
            x, y,
            cw * scalex, ch * scaley, FlipNone);

        x += (cw + xoff) * scalex;
    } 
}


// Fill a rectangle
void g_fill_rect(Graphics* g, float dx, float dy, 
    float dw, float dh, Color c) {

    SDL_SetRenderDrawColor(g->rend, c.r, c.g, c.b, c.a);

    apply_transform(g, &dx, &dy, &dw, &dh);

    // Draw a filled rectangle
    SDL_Rect dest = (SDL_Rect) {
        (int)dx, (int)dy, (int)dw, (int)dh
    };
    SDL_RenderFillRect(g->rend, &dest);
}
