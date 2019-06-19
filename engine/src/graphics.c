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
    g->canvasTarget.x = conf_get_param_int(conf, "canvas_width", 256);
    g->canvasTarget.y = conf_get_param_int(conf, "canvas_height", 192);

    // Create texture
    g->canvas = create_bitmap((uint16)g->canvasTarget.x, 
        (uint16)g->canvasTarget.y, true);
    if(g->canvas == NULL) {

        dispose_graphics(g);
        return NULL;
    }

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
    if(g->canvas != NULL) {

        destroy_bitmap(g->canvas);
    }

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

    int newAspect = (int)w / (int)h;
    int targetAspect = (int)g->canvasTarget.x / (int)g->canvasTarget.y;
    int mod = 0;

    // If the same aspect ratio or wider
    if(newAspect >= targetAspect) {

        mod =  h / g->canvasTarget.y;
    }
    else {

        mod =  w / g->canvasTarget.x;
    }

    g->canvasScale.x = mod * g->canvasTarget.x;
    g->canvasScale.y = mod * g->canvasTarget.y;

    g->canvasPos.x = w/2 - g->canvasScale.x/2;
    g->canvasPos.y = h/2 - g->canvasScale.y/2;

    // Store window size
    g->windowSize = point(w, h);
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


// Toggle canvas target
void g_toggle_canvas_target(Graphics* g, bool state) {

    SDL_SetRenderTarget(g->rend, state ? g->canvas->tex : NULL);
}


// Refresh
void g_refresh(Graphics* g) {

    g_set_blend_color(g, rgb(255, 255, 255));

    // Draw the canvas
    g_draw_scaled_bitmap(g, g->canvas, 
        g->canvasPos.x, g->canvasPos.y,
        g->canvasScale.x, g->canvasScale.y, FlipNone);

    // Render the frame
    SDL_RenderPresent(g->rend);
}


// Clear screen
void g_clear_screen(Graphics* gr, uint8 r, uint8 g, uint8 b) {

    SDL_SetRenderDrawColor(gr->rend, r, g, b, 255);
    SDL_RenderClear(gr->rend);
}


// Draw a bitmap
void g_draw_bitmap(Graphics* g, Bitmap* bmp, 
    int dx, int dy, int flip) {

    g_draw_scaled_bitmap_region(g, bmp,
        0, 0, bmp->width, bmp->height, 
        dx, dy, bmp->width, bmp->height, 
        flip);
}


// Draw a scaled bitmap
void g_draw_scaled_bitmap(Graphics* g, Bitmap* bmp, 
   int dx, int dy, int dw, int dh, int flip) {

    g_draw_scaled_bitmap_region(g, bmp,
        0, 0, bmp->width, bmp->height, 
        dx, dy, dw, dh, flip);
}


// Draw a bitmap region
void g_draw_bitmap_region(Graphics* g, Bitmap* bmp, 
    int sx, int sy, int sw, int sh, 
    int dx, int dy, int flip) {

    g_draw_scaled_bitmap_region(g, bmp, sx, sy, sw, sh,
        dx, dy, sw, sh, flip);
}


// Draw a scaled bitmap region
void g_draw_scaled_bitmap_region(Graphics* g, Bitmap* bmp, 
    int sx, int sy, int sw, int sh, 
    int dx, int dy, int dw, int dh,
    int flip) {

    // Apply blend color
    SDL_SetTextureColorMod(bmp->tex, 
        g->blendColor.r, g->blendColor.g, g->blendColor.b);
    SDL_SetTextureAlphaMod(bmp->tex, g->blendColor.a);

    // Set source & destination
    SDL_Rect src = (SDL_Rect){sx, sy, sw, sh};
    SDL_Rect dst = (SDL_Rect){dx, dy, dw, dh};

    // Draw
    SDL_RenderCopyEx(g->rend, bmp->tex, &src, &dst,
            0.0, NULL, (SDL_RendererFlip)flip);
}


// Draw scaled text
void g_draw_text(Graphics* g, Bitmap* bmp, const char* text,
    int dx, int dy, int xoff, int yoff, 
    bool center) {

    int cw = (bmp->width) / 16;
    int ch = cw;
    int len = strlen(text);

    int x = dx;
    int y = dy;
    unsigned char c;

    // Center the text
    if (center)
    {
        dx -= ((len + 1) / 2.0f * (cw + xoff));
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
            y += (yoff + ch);
            continue;
        }

        sx = c % 16;
        sy = (c / 16);
        // Draw character
        g_draw_bitmap_region(g, 
            bmp, sx * cw, sy * ch, cw, ch,
            x, y, FlipNone);

        x += (cw + xoff);
    } 
}


// Fill a rectangle
void g_fill_rect(Graphics* g, int dx, int dy, 
    int dw, int dh, Color c) {

    SDL_SetRenderDrawColor(g->rend, c.r, c.g, c.b, c.a);

    // Draw a filled rectangle
    SDL_Rect dest = (SDL_Rect) {
        dx, dy, dw, dh
    };
    SDL_RenderFillRect(g->rend, &dest);
}
