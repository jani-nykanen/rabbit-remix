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


// Clip a rectangle
static bool clip_rect(Graphics* g, int* x, int* y, 
    int* w, int* h) {

    // Left
    if(*x < 0) {

        *w -= 0 - (*x);
        *x = 0;
    }
    // Right
    if(*x+*w >= g->csize.x) {

        *w -= (*x+*w) - (0 + g->csize.x);
    }

    // Top
    if(*y < 0) {

        *h -= 0 - (*y);
        *y = 0;
    }
    // Bottom
    if(*y+*h >= g->csize.y) {

        *h -= (*y+*h) - (0 + g->csize.y);
    }

    return *w > 0 && *h > 0;
}


// Clip (general)
static bool clip(Graphics* g, int* sx, int* sy, int* sw, int* sh, 
    int* dx, int* dy, bool flip) {

    int ow, oh;

    // Left
    ow = *sw;
    if(*dx < 0) {

        *sw -= 0 - (*dx);
        if(!flip)
            *sx += ow-*sw;

        *dx = 0;
    }
    // Right
    if(*dx+*sw >= g->csize.x) {

         *sw -= (*dx+*sw) - (0 + g->csize.x); 
         if(flip)
            *sx += ow-*sw;
    }

    // Top
    oh = *sh;
    if(*dy < 0) {

        *sh -= 0 - (*dy);
        *sy += oh-*sh;
        *dy = 0;
    }
    // Bottom
    if(*dy+*sh >= g->csize.y) {

        *sh -= (*dy+*sh) - (0 + g->csize.y);
    }

    return *sw > 0 && *sh > 0;
}


// Create a graphics component
Graphics* create_graphics(SDL_Window* window, Config* conf) {

    // Allocate memory
    Graphics* g = (Graphics*)malloc(sizeof(Graphics));
    if (g == NULL) {

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
    if (g->rend == NULL) {

        err_throw_no_param("Failed to create a renderer.");
        return NULL;
    }

    // Pass renderer to the bitmap loader
    init_bitmap_loader(g->rend);

    // Read view target size
    g->csize.x = conf_get_param_int(conf, "canvas_width", 256);
    g->csize.y = conf_get_param_int(conf, "canvas_height", 192);

    // Create the canvas texture
    g->canvas = 
        SDL_CreateTexture(g->rend, 
        SDL_PIXELFORMAT_RGB332, 
        SDL_TEXTUREACCESS_STREAMING, 
        g->csize.x, 
        g->csize.y);
    if (g->canvas == NULL) {

        err_throw_param_1("Failed to create a texture: ", SDL_GetError());
        return NULL;
    }

    // Create canvas data
    g->pdata = (uint8*)malloc(sizeof(uint8)*g->csize.x*g->csize.y);
    if (g->pdata == NULL) {

        free(g);
        ERR_MEM_ALLOC;
        return NULL;
    }

    // Resize
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    g_resize(g, w, h);

    return g;
}


// Dispose graphics
void dispose_graphics(Graphics* g) {

    if (g == NULL) return;

    SDL_DestroyRenderer(g->rend);
    SDL_DestroyTexture(g->canvas);

    free(g->pdata);
    free(g);
}


// Resize event
void g_resize(Graphics* g, int w, int h) {

    int newAspect = (int)w / (int)h;
    int targetAspect = (int)g->csize.x / (int)g->csize.y;
    int mod = 0;

    // If the same aspect ratio or wider
    if (newAspect >= targetAspect) {

        mod =  h / g->csize.y;
    }
    else {

        mod =  w / g->csize.x;
    }

    g->canvasScale.x = mod * g->csize.x;
    g->canvasScale.y = mod * g->csize.y;

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


// Pass data to the canvas
void g_update_pixel_data(Graphics* g) {
    
    SDL_UpdateTexture(g->canvas, NULL, g->pdata, g->csize.x);
}


// Refresh
void g_refresh(Graphics* g) {

    // Clear background
    SDL_SetRenderDrawColor(g->rend, 0, 0, 0, 255);
    SDL_RenderClear(g->rend);

    // Set source & destination
    SDL_Rect dst = (SDL_Rect) { 
        g->canvasPos.x, g->canvasPos.y,
        g->canvasScale.x, g->canvasScale.y
    };
    // Draw canvas
    SDL_RenderCopy(g->rend, g->canvas, NULL, &dst);

    // Render the g->pdata
    SDL_RenderPresent(g->rend);
}


// Clear screen
void g_clear_screen(Graphics* g, uint8 c) {

    int32 i = 0;
    for(; i < g->csize.x*g->csize.y; ++ i) {

        g->pdata[i] = c;
    }
}


// Draw some static
void g_draw_static(Graphics* g) {

    int32 i = 0;
    for(; i < g->csize.x*g->csize.y; ++ i) {

        g->pdata[i] = (rand() % 2 == 0) ? 255 : 0;
    }
}


// Draw a bitmap
void g_draw_bitmap(Graphics* g, Bitmap* bmp, 
    int dx, int dy, bool flip) {

    g_draw_scaled_bitmap_region(g, bmp,
        0, 0, bmp->width, bmp->height, 
        dx, dy, 
        flip);
}


// Draw a bitmap region
void g_draw_bitmap_region(Graphics* g, Bitmap* bmp, 
    int sx, int sy, int sw, int sh, 
    int dx, int dy, bool flip) {

    g_draw_scaled_bitmap_region(g, bmp, sx, sy, sw, sh,
        dx, dy, flip);
}


// Draw a scaled bitmap region
void g_draw_scaled_bitmap_region(Graphics* g, Bitmap* bmp, 
    int sx, int sy, int sw, int sh, 
    int dx, int dy,
    bool flip) {

    int x, y;
    uint64 offset;
    int boff;
    int pixel;
    int dir = flip ? -1 : 1;

    if (bmp == NULL) return;

    // Translate
    dx += g->translation.x;
    dy += g->translation.y;

    // Clip
    if(!clip(g, &sx, &sy, &sw, &sh, &dx, &dy, flip))
        return;

    // Draw pixels
    offset = g->csize.x*dy + dx;
    boff = bmp->width*sy + sx + (flip ? (sw-1) : 0);
    for(y = 0; y < sh; ++ y) {

        for(x = 0; x < sw; ++ x) {

            pixel = bmp->data[boff];
            // Check if not alpha pixel
            // (i.e not transparent)
            if (pixel != ALPHA) {

                g->pdata[offset] = pixel;
            }

            boff += dir;
            ++ offset;
        }
        boff += bmp->width-sw*dir;
        offset += g->csize.x-sw;
    }
}


// Draw a bitmap
void g_draw_bitmap_fast(Graphics* g, Bitmap* bmp, 
    int dx, int dy) {

    g_draw_scaled_bitmap_region_fast(g, bmp,
        0, 0, bmp->width, bmp->height, 
        dx, dy);
}


// Draw a bitmap region
void g_draw_bitmap_region_fast(Graphics* g, Bitmap* bmp, 
    int sx, int sy, int sw, int sh, 
    int dx, int dy) {

    g_draw_scaled_bitmap_region_fast(g, bmp, sx, sy, sw, sh,
        dx, dy);
}


// Draw a scaled bitmap region fast
void g_draw_scaled_bitmap_region_fast(Graphics* g, Bitmap* bmp, 
    int sx, int sy, int sw, int sh, 
    int dx, int dy) {

    int x, y;
    uint64 offset;
    int boff;
    int pixel;

    if (bmp == NULL) return;

    // Translate
    dx += g->translation.x;
    dy += g->translation.y;

    // Clip
    if (!clip(g, &sx, &sy, &sw, &sh, &dx, &dy, false))
        return;

    // Draw pixels
    offset = g->csize.x*dy + dx;
    boff = bmp->width*sy + sx +1;
    for (y = dy; y < dy+sh; ++ y) {

        memcpy(g->pdata + offset, bmp->data + boff, sw);
        offset += g->csize.x;
        boff += bmp->width;
    }
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
    if (center) {

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
    int dw, int dh, uint8 col) {

    int y;
    uint64 offset;

    dx += g->translation.x;
    dy += g->translation.y;

    // Clip
    if (!clip_rect(g, &dx, &dy, &dw, &dh))
        return;

    // Draw
    offset = g->csize.x*dy + dx;
    for(y = dy; y < dy+dh; ++ y) {

        memset(g->pdata + offset, col, dw);
        offset += (int64) g->csize.x;
    }
}
