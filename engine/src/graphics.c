#include "graphics.h"

#include "bitmap.h"
#include "err.h"
#include "mathext.h"

// Maximum value for darkness
#define MAX_DARKNESS_VALUE 8

// Fixed point math precision
static const int FIXED_PREC = 256;

// Global darkness palettes
static uint8 dpalette [MAX_DARKNESS_VALUE] [256];
// Dithering arrays
static uint8 ditherArray [MAX_DARKNESS_VALUE*2] [2];


// Get darkened color index
static uint8 get_darkened_color(uint8 col, int amount)
{
    uint8 r,g,b;

    r = col >> 5;
    g = col << 3;
    g = g >> 5;
    b = col << 6;
    b = b >> 6; 

    int i = 0;
    for (; i < amount; ++ i)
    {
        if (r > 0) 
            -- r;
        if (g > 0) 
            -- g;
        
        if (i % 2 == 1) {

            if (b > 0) 
                -- b;
        }
    }

    r = r << 5;
    g = g << 2;

    return r | g | b;
}


// Generate darkness palettes
static void gen_darkness_palettes() {

    int i, j;

    for(i = 0; i < MAX_DARKNESS_VALUE; ++ i)
    {
        for(j = 0; j < 256; ++ j)
        {
            dpalette[i][j] = get_darkened_color(j, i);
        }
    }
}


// Generate dithering array
static void gen_dither_array() {

    int i, j;

    for (i = 0; i < MAX_DARKNESS_VALUE*2; ++ i) {

        j = i / 2;

        ditherArray[i][0] = i % 2 == 0 ? j : j+1;
        ditherArray[i][1] = j;
    }
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


// Draw half a triangle
static void draw_triangle_half(Graphics * g,
    int midx, int midy, int endMid, int endy, 
    int stepx, int stepy, int dx, int dend, 
    int x1, int y1, 
    uint8 col) {

    int sx = midx * FIXED_PREC;
    int ex = endMid * FIXED_PREC;

    int x, y;
    uint64 offset;

    int signx = stepx > 0 ? 1 : -1;
    int signy = stepy > 0 ? 1 : -1;

    int endx;

    // Draw horizontal lines
    for (y = midy; y*signy <= endy*signy; y += stepy) {

        x = sx / FIXED_PREC;
        offset = y * g->csize.x + x;

        endx = ex / FIXED_PREC;
        for (; x*signx <= endx*signx; x += stepx) {

            // Check if inside the canvas
            if (x >= 0 
                && x < g->csize.x && y >= 0 
                && y < g->csize.y) {
                
                g->pdata[offset] = dpalette[ ditherArray[g->dvalue] [x % 2 == y % 2] ] [col];
            }
            offset += stepx;
        }

        sx += dx;
        ex -= dend;
    }
}


// Initialize global graphics content
int init_graphics_global() {

    // Initialize global arrays
    gen_darkness_palettes();
    gen_dither_array();

    return 0;
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

    // Read view target size
    g->csize.x = conf_get_param_int(conf, "canvas_width", 256);
    g->csize.y = conf_get_param_int(conf, "canvas_height", 192);
    g->aspectRatio = (float)g->csize.x / (float)g->csize.y;

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

    // Set defaults
    g->translation = point(0, 0);
    g->dvalue = 0;

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

    g_draw_bitmap_region(g, bmp,
        0, 0, bmp->width, bmp->height, 
        dx, dy, 
        flip);
}


// Draw a bitmap region
void g_draw_bitmap_region(Graphics* g, Bitmap* bmp, 
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


// Draw a scaled bitmap region
void g_draw_scaled_bitmap_region(Graphics* g, Bitmap* bmp, 
    int sx, int sy, int sw, int sh, 
    int dx, int dy, int dw, int dh,
    bool flip) {

    //
    // To get a better performance, we don't
    // make draw_bitmap_region function call
    // this function, thus we rewrite a lot of
    // code.
    //

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
    if(!clip(g, &sx, &sy, &dw, &dh, &dx, &dy, flip))
       return;

    int tx, ty;
    uint8 col;

    // Jumps
    int xjump = sw * FIXED_PREC / dw;
    int yjump = sh * FIXED_PREC / dh;

    // Draw pixels
    ty = sy * FIXED_PREC;
    for(y = dy; y < dy+dh; ++ y) {

        tx = sx * FIXED_PREC;
        for(x = dx; x < dx+dw; ++ x) {

            col = bmp->data[ 
                round_fixed(ty, FIXED_PREC)*bmp->width 
                + round_fixed(tx, FIXED_PREC) ];
            
            if (col != ALPHA) {

                // TODO:
                // Pixel put function
                col = g->pdata[y*g->csize.x + x];
                g->pdata[y*g->csize.x + x] 
                    = dpalette[ ditherArray[g->dvalue] [ x % 2 == y % 2] ] [col];

                // g->pdata[ y*g->csize.x + x ] = col;
            }

            tx += xjump;
        }

        ty += yjump;
    }
}


// Draw a bitmap
void g_draw_bitmap_fast(Graphics* g, Bitmap* bmp, 
    int dx, int dy) {

    g_draw_bitmap_region_fast(g, bmp,
        0, 0, bmp->width, bmp->height, 
        dx, dy);
}


// Draw a scaled bitmap region fast
void g_draw_bitmap_region_fast(Graphics* g, Bitmap* bmp, 
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
    boff = bmp->width*sy + sx;
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


// Draw a triangle
void g_draw_triangle(Graphics* g, 
    int x1, int y1, 
    int x2, int y2, 
    int x3, int y3, 
    uint8 col) {

    // Sort points
    Point points[3];
    points[0] = point(x1, y1);
    points[1] = point(x2, y2);
    points[2] = point(x3, y3);
    sort_points_3(points);

    // If not in the screen, do not draw
    if (points[0].y >= g->csize.y-1 || points[2].y < 0 ||
        min_int32_3(x1, x2, x3) >= g->csize.x-1 || 
        max_int32_3(x1, x2 ,x3) < 0) {

        return;
    }

    // Calculate horizontal step(s)
    int dx1, dx2, dend;
    if (points[1].y != points[0].y)
        dx1 = -(points[1].x - points[0].x)*FIXED_PREC / 
               (points[1].y - points[0].y);
    else
        dx1 = 0;

    if (points[1].y != points[2].y)
        dx2 =  (points[1].x - points[2].x)*FIXED_PREC / 
               (points[1].y - points[2].y);
    else 
        dx2 = 0;
    
    dend = (points[2].x - points[0].x)*FIXED_PREC / 
           (points[2].y - points[0].y);

    // "End middle" is the y coordinate in the "end line" (i.e
    //  where every rendered line ends)
    int endMid = points[0].x + 
        (dend * (points[1].y - points[0].y)) 
        / FIXED_PREC;
    
    // Set horizontal step
    int stepx = points[1].x < endMid ? 1 : -1;
    
    // Draw the upper half
    if (points[1].y != points[0].y) {
        
        draw_triangle_half(g,
            points[1].x,points[1].y, endMid, 
            max_int32_2(points[0].y, -1), stepx, -1, 
            dx1, dend, x1, y1, col);
    }

    // Draw the bottom half
    if (points[1].y != points[2].y) {

        draw_triangle_half(g, 
            points[1].x,points[1].y, endMid, 
            min_int32_2(points[2].y, g->csize.y), stepx, 1, 
            dx2, -dend, x1, y1, col);
    }
}


// Draw a line
void g_draw_line(Graphics* g, int x1, int y1, 
    int x2, int y2, uint8 col) {

    // Bresenham's line algorithm
    int dx = abs(x2-x1), sx = x1<x2 ? 1 : -1;
    int dy = abs(y2-y1), sy = y1<y2 ? 1 : -1; 
    int err = (dx>dy ? dx : -dy)/2, e2;
     
    while(true) {

        if (!(y1 >= g->csize.y-1 || y1 < 0 ||
            x1 >= g->csize.x-1 || x1 < 0 )) {

            g->pdata[y1 * g->csize.x + x1] = col;
        }
        
        if (x1 == x2 && y1 == y2) 
            break;

        e2 = err;
        if (e2 >-dx) { 
            err -= dy; x1 += sx; 
        }
        if (e2 < dy) { 
            err += dx; y1 += sy; 
        }
    }
}


// Draw "3D" floor
void g_draw_3D_floor(Graphics* g, Bitmap* bmp,
    int dx, int dy, int w, int h, int xdelta,
    float mx, float my) {

    //
    // TODO: Use fixed point numbers
    // for slightly better performance
    //

    int x, y;
    int tx, ty;
    int px, py;
    uint8 col;

    int yjumpDelta = (int)(mx*FIXED_PREC) / h;
    int yjump = FIXED_PREC;

    int xjumpDelta = (int)((my-1.0f)*FIXED_PREC) / h;
    int xjump = (int)(my*FIXED_PREC);

    ty = 0;
    for (y = dy; y < dy + h; ++ y) {

        py = round_fixed(ty, FIXED_PREC) % bmp->height;
        tx = -w/2 * yjump;
        for (x = dx; x < dx + w; ++ x) {
            
            px = neg_mod((round_fixed(tx, FIXED_PREC))-xdelta, bmp->width);
            col = bmp->data[py * bmp->width + px];

            g->pdata[y*g->csize.x + x] = col;

            tx += yjump;
        }
        ty += xjump;

        xjump -= xjumpDelta;
        yjump -= yjumpDelta;
        
    }    
}
