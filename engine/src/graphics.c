#include "graphics.h"

#include "bitmap.h"
#include "err.h"
#include "mathext.h"

#include <math.h>

// Maximum value for darkness & light effects
#define MAX_PALETTE_MOD 8

// Global darkness & light palettes (aaaand red)
static uint8** dpalette;
static uint8** lpalette;
static uint8** redPalette;
// Dithering array
static uint8 ditherArray [MAX_PALETTE_MOD*2] [2];


//
// Pixel functions
//
static void pfunc_default(void* _g, int offset, uint8 col) {

    Graphics* g = (Graphics*)_g;
    g->pdata[offset] = col;
}
static void pfunc_darken(void* _g, int offset, uint8 c) {

    Graphics* g = (Graphics*)_g;
    uint8 col = g->pdata[offset];
    int x = offset % g->csize.x;
    int y = offset / g->csize.x;

    g->pdata[offset] 
         = dpalette[ ditherArray[g->pparam1] [ x % 2 == y % 2] ] [col];
}
static void pfunc_single_color(void* _g, int offset, uint8 col) {

    Graphics* g = (Graphics*)_g;
    g->pdata[offset] = g->pparam1;
}
static void pfunc_skip_single_color(void* _g, int offset, uint8 col) {

    Graphics* g = (Graphics*)_g;

    int x = offset % g->csize.x;
    int y = offset / g->csize.x;

    if (x % g->pparam1 == 0 || y % g->pparam1 == 0) 
        return;
    g->pdata[offset] = g->pparam2;
}
static void pfunc_inverse_frame(void* _g, int offset, uint8 col) {

    Graphics* g = (Graphics*)_g;
    g->pdata[offset] = ~g->pdata[offset];
}
static void pfunc_inverse(void* _g, int offset, uint8 col) {

    Graphics* g = (Graphics*)_g;
    g->pdata[offset] = ~col;
}
static void pfunc_skip_inverse(void* _g, int offset, uint8 col) {

    Graphics* g = (Graphics*)_g;

    int x = offset % g->csize.x;
    int y = offset / g->csize.x;

    if (x % g->pparam1 == 0 || y % g->pparam1 == 0) 
        return;
    g->pdata[offset] = ~g->pdata[offset];
}
static void pfunc_skip(void* _g, int offset, uint8 col) {

    Graphics* g = (Graphics*)_g;

    int x = offset % g->csize.x;
    int y = offset / g->csize.x;

    if (x % g->pparam1 == 0 || y % g->pparam1 == 0) 
        return;
    g->pdata[offset] = col;
}
static void pfunc_skip_simple(void* _g, int offset, uint8 col) {

    Graphics* g = (Graphics*)_g;

    int x = offset % g->csize.x;
    int y = offset / g->csize.x;

    if (x % 2 == y % 2) 
        g->pdata[offset] = col;
}
static void pfunc_skip_simple_single_color(void* _g, int offset, uint8 col) {

    Graphics* g = (Graphics*)_g;

    int x = offset % g->csize.x;
    int y = offset / g->csize.x;

    if (x % 2 == y % 2) 
        g->pdata[offset] = g->pparam2;
}
static void pfunc_lighten(void* _g, int offset, uint8 c) {

    Graphics* g = (Graphics*)_g;
    uint8 col = g->pdata[offset];
    int x = offset % g->csize.x;
    int y = offset / g->csize.x;

    g->pdata[offset] 
         = lpalette[ ditherArray[g->pparam1] [ x % 2 == y % 2] ] [col];
}
static void pfunc_texture(void* _g, int offset, uint8 col) {

    Graphics* g = (Graphics*)_g;

    int x = offset % g->csize.x;
    int y = offset / g->csize.x;

    int dx = x - g->top.x;
    int dy = y - g->top.y;

    Point t = point(dx, dy);

    // Get texture coordinates
    t = mat2_fixed_mul(g->uvTransf, t);

    t.x += g->uvTrans.x;
    t.y += g->uvTrans.y;

    // Limit the coordinates inside the _BITMAP
    t.x = neg_mod(t.x, g->tex->width);
    t.y = neg_mod(t.y, g->tex->height);

    col = g->tex->data[t.y * g->tex->width + t.x];
    if (col != ALPHA)
        g->pdata[offset] = col;
}


// Generate texturing matrices etc.
static void gen_uv_transf(Graphics* g, Bitmap* tex,
    int x1, int y1, int x2, int y2, int x3, int y3) {
    
    if (tex == NULL) return;

    g->tex = tex;
    g->top = point(x1, y1);

    float w = (float) tex->width;
    float h = (float) tex->height;

    // Generate UV transform matrix
    Matrix2 uv = mat2(
        (g->uv3.x - g->uv1.x), (g->uv2.x - g->uv1.x),
        (g->uv3.y - g->uv1.y), (g->uv2.y - g->uv1.y)
    );
    uv = mat2_inverse(uv);

    Vector2 u = vec2((float)(x3-x1), (float)(y3-y1));
    Vector2 v = vec2((float)(x2-x1), (float)(y2-y1));

    Matrix2 B = mat2(
        u.x, v.x,
        u.y, v.y
    );
    Matrix2 S = mat2(
        1.0f/w, 0.0f,
        0.0f, 1.0f/h
    );

    g->uvTrans = point(
        (int)roundf(g->uv1.x * w), 
        (int)roundf(g->uv1.y * h)
    );

    // Final matrix
    Matrix2 m = mat2_mul(B, uv);
    m = mat2_mul(S, m);
    m = mat2_inverse(m);

    g->uvTransf = mat2_to_fixed(m);
}


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


// Get lightened color index
static uint8 get_lightened_color(uint8 col, int amount)
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
        if (r < 7) 
            ++ r;
        if (g < 7) 
            ++ g;
        
        if (i % 2 == 1) {

            if (b < 3) 
                ++ b;
        }
    }

    r = r << 5;
    g = g << 2;

    return r | g | b;
}


// Get red...dened? color index
static uint8 get_red_color(uint8 col, int amount)
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
        if (r < 7) 
            ++ r;
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



// Generate darkness & light palettes
static int gen_palette_mods() {

    int i, j;

    // Allocate memory
    dpalette = (uint8**)malloc(sizeof(uint8**) * MAX_PALETTE_MOD);
    lpalette = (uint8**)malloc(sizeof(uint8**) * MAX_PALETTE_MOD);
    redPalette = (uint8**)malloc(sizeof(uint8**) * MAX_PALETTE_MOD);
    if (dpalette == NULL || lpalette == NULL || redPalette == NULL) {

        ERR_MEM_ALLOC;
        return 1;
    }

    for (i = 0; i < MAX_PALETTE_MOD; ++ i) {

        dpalette[i] = (uint8*)malloc(sizeof(uint8) * 256);
        lpalette[i] = (uint8*)malloc(sizeof(uint8) * 256);
        redPalette[i] = (uint8*)malloc(sizeof(uint8) * 256);

        if (dpalette[i] == NULL || lpalette[i] == NULL || redPalette[i] == NULL) {

            ERR_MEM_ALLOC;
            return 1;
        }
    }

    for(i = 0; i < MAX_PALETTE_MOD; ++ i)
    {
        for(j = 0; j < 256; ++ j)
        {
            dpalette[i][j] = get_darkened_color(j, i);
            lpalette[i][j] = get_lightened_color(j, i);
            redPalette[i][j] = get_red_color(j, i);
        }
    }

    return 0;
}


// Generate dithering array
static void gen_dither_array() {

    int i, j;

    for (i = 0; i < MAX_PALETTE_MOD*2; ++ i) {

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
                && x < g->csize.x 
                && y >= 0 
                && y < g->csize.y) {
                
                g->pfunc(g, offset, col);
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
    if( gen_palette_mods() == 1) {

        return 1;
    }
    gen_dither_array();

    return 0;
}


// Destroy global graphics
void destroy_global_graphics() {

    int i = 0;
    for (i = 0; i < MAX_PALETTE_MOD; ++ i) {

        free(dpalette[i]);
        free(lpalette[i]);
        free(redPalette[i]);
    }

    free(dpalette);
    free(lpalette);
    free(redPalette);
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
    // Create canvas data buffer
    g->pbuffer = (uint8*)malloc(sizeof(uint8)*g->csize.x*g->csize.y);
    if (g->pbuffer == NULL) {

        free(g->pdata);
        free(g);
        ERR_MEM_ALLOC;
        return NULL;
    }
    g->bufferCopy.width = g->csize.x;
    g->bufferCopy.height = g->csize.y;
    g->bufferCopy.data = g->pbuffer;

    // Resize
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    g_resize(g, w, h);

    // Set defaults
    g->translation = point(0, 0);
    g->dvalue = 0;
    g->pfunc = pfunc_default;
    g->pparam1 = 0;
    g->tex = NULL;
    g->darray = dpalette;

    return g;
}


// Dispose graphics
void dispose_graphics(Graphics* g) {

    if (g == NULL) return;

    SDL_DestroyRenderer(g->rend);
    SDL_DestroyTexture(g->canvas);

    free(g->pbuffer);
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


// Set pixel function
void g_set_pixel_function(Graphics* g, int func, int param1, int param2) {

    // TODO: An array approach
    switch (func)
    {
    case PixelFunctionDefault:
        g->pfunc = pfunc_default;
        break;

    case PixelFunctionDarken:
        g->pfunc = pfunc_darken;
        break;

    case PixelFunctionSingleColor:
        g->pfunc = pfunc_single_color;
        break;  

    case PixelFunctionSingleColorSkip:
        g->pfunc = pfunc_skip_single_color;
        break;

    case PixelFunctionInverseFrame:
        g->pfunc = pfunc_inverse_frame;
        break;    

    case PixelFunctionInverse:
        g->pfunc = pfunc_inverse;
        break;    

    case PixelFunctionInverseSkip:
        g->pfunc = pfunc_skip_inverse;
        break;

    case PixelFunctionSkip:
        g->pfunc = pfunc_skip;
        break;

    case PixelFunctionSkipSimple:
        g->pfunc = pfunc_skip_simple;
        break;

    case PixelFunctionSingleColorSkipSimple:
        g->pfunc = pfunc_skip_simple_single_color;
        break;

    case PixelFunctionLighten:
        g->pfunc = pfunc_lighten;
        break;
    
    default:
        break;
    }
    g->pparam1 = param1;
    g->pparam2 = param2;
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
    int offset;
    int boff;
    int pixel;
    int dir = flip ? -1 : 1;

    if (bmp == NULL) return;

    // Translate
    dx += g->translation.x;
    dy += g->translation.y;

    // Clip the source rectangle
    // TODO: Separate function for this...?
    if (sx < 0) {

        sw += sx;
        sx = 0;
    }
    if (sx + sw > bmp->width) {

        sw -= (sx+sw) - (bmp->width);
    }
    if (sy < 0) {

        sh += sy;
        sy = 0;
    }
    if (sy + sh > bmp->height) {

        sh -= (sy+sh) - (bmp->height);
    }
    if (sw == 0 || sh == 0) return;

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

                g->pfunc(g, offset, pixel);
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
    int offset;
    int boff;
    int pixel;
    int dir = flip ? -1 : 1;

    if (bmp == NULL || dw <= 0 || dh <= 0) return;

    // Translate
    dx += g->translation.x;
    dy += g->translation.y;

    // If outside the screen, do not draw
    if (dx+dw < 0 || dy+dh < 0 ||
        dx >= g->csize.x || dy >= g->csize.y  )
        return;

    int tx, ty;
    uint8 col;

    // Jumps
    int xjump = sw * FIXED_PREC / dw;
    int yjump = sh * FIXED_PREC / dh;

    // Draw pixels
    ty = sy * FIXED_PREC;
    for(y = dy; y < min_int32_2(dy+dh, g->csize.y); ++ y) {

        if (y < 0) {

            ty += yjump * -y;
            y = 0;
        }

        tx = (flip ? sx+sw-1 : sx) * FIXED_PREC;
        for(x = dx; x < min_int32_2(dx+dw, g->csize.x); ++ x) {

            if (x < 0) {

                tx += dir * xjump * -x;
                x = 0;
            }

            col = bmp->data[ 
                min_int32_2(round_fixed(ty, FIXED_PREC), bmp->height-1)*bmp->width  +
                min_int32_2(round_fixed(tx, FIXED_PREC), bmp->width-1) ];
            
            if (col != ALPHA) {

                g->pfunc(g, y*g->csize.x+x, col);
            }

            tx += xjump * dir;
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

        dx -= (len + 1) * (cw + xoff) / 2;
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
            x, y, false);

        x += (cw + xoff);
    } 
}


// Fill a rectangle
void g_fill_rect(Graphics* g, int dx, int dy, 
    int dw, int dh, uint8 col) {

    int x, y;
    uint64 offset;

    dx += g->translation.x;
    dy += g->translation.y;

    // Clip
    if (!clip_rect(g, &dx, &dy, &dw, &dh))
        return;

    // Draw
    offset = g->csize.x*dy + dx;
    for (y = dy; y < dy+dh; ++ y) {

        for (x = dx; x < dx+dw; ++ x) {

            g->pfunc(g, offset ++, col);
        }
        offset += g->csize.x-dw;
    }
}


// Draw a triangle
void g_draw_triangle(Graphics* g, 
    int x1, int y1, 
    int x2, int y2, 
    int x3, int y3, 
    uint8 col) {

    // If points are in the same line, do not draw
    if ( abs((x3-x1)*(y3-y1)-(x2-x1)*(y2-y1)) == 0 ) {

        return;
    }

    // Translate
    x1 += g->translation.x;
    y1 += g->translation.y;
    x2 += g->translation.x;
    y2 += g->translation.y;
    x3 += g->translation.x;
    y3 += g->translation.y;

    // Sort points
    Point points[3];
    points[0] = point(x1, y1);
    points[1] = point(x2, y2);
    points[2] = point(x3, y3);
    sort_points_3(points);

    // If not on the screen, do not draw
    if (points[0].y >= g->csize.y-1 || points[2].y < 0 ||
        min_int32_3(x1, x2, x3) >= g->csize.x-1 || 
        max_int32_3(x1, x2 ,x3) < 0) {

        return;
    }

    if (g->tex != NULL) {

        // Generate uv transform matrix
        gen_uv_transf(g, g->tex, 
            x1, y1, x2, y2, x3, y3);

        // Use the texturing function now
        g->pfunc = pfunc_texture; 
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
    
    // TODO: Maybe set dend = 0?
    if ((points[2].y - points[0].y) == 0)
        return;

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
            points[1].x, points[1].y, endMid, 
            max_int32_2(points[0].y-1, -1), stepx, -1, 
            dx1, dend, col);
    }

    // Draw the bottom half
    if (points[1].y != points[2].y) {

        draw_triangle_half(g, 
            points[1].x,points[1].y+1, endMid, 
            min_int32_2(points[2].y, g->csize.y), stepx, 1, 
            dx2, -dend, col);
    }

    if (g->tex != NULL) {

        g->pfunc = pfunc_default;
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


// Draw a thick line
void g_draw_thick_line(Graphics* g, 
    int dx1, int dy1, int dx2, int dy2, int r, uint8 col) {

    float angle = atan2f(dy2-dy1, dx2-dx1) + M_PI/2.0f;

    float c = cosf(angle);
    float s = sinf(angle);

    int x1 = dx1-(int)(c*r/2);
    int y1 = dy1-(int)(s*r/2);

    int x2 = dx2-(int)(c*r/2);
    int y2 = dy2-(int)(s*r/2);

    int x3 = dx2+(int)(c*r/2);
    int y3 = dy2+(int)(s*r/2);

    int x4 = dx1+(int)(c*r/2);
    int y4 = dy1+(int)(s*r/2);

    g_draw_triangle(g, x1, y1, x2, y2, x3, y3, col);
    g_draw_triangle(g, x3, y3, x4, y4, x1, y1, col);
} 


// Draw "3D" floor
void g_draw_3D_floor(Graphics* g, Bitmap* bmp,
    int dx, int dy, int w, int h, int xdelta,
    int mx, int my) {

    int x, y;
    int tx, ty;
    int px, py;
    uint8 col;

    // Translate
    dx += g->translation.x;
    dy += g->translation.y;
    w -= g->translation.x;
    h -= g->translation.y;

    int yjumpDelta = (int)mx / h;
    int yjump = FIXED_PREC;

    int xjumpDelta = (int)(my-FIXED_PREC) / h;
    int xjump = (int)my;

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


// Set UV coordinates
void g_set_uv_coords(Graphics* g, 
    float u1, float v1, 
    float u2, float v2, 
    float u3, float v3) {

    const float EPS = 0.001f;

    // Check if coordinates are linearly dependable
    if ( fabsf((u3-u1)*(v3-v1) - (u2-u1)*(v2-v1)) < EPS ) {

        // Make the coordinates "small"
        u1 = 0; v1 = 0;
        u2 = EPS; v2 = EPS;
        u3 = 0; v3 = EPS;
    }

    g->uv1 = vec2(u1, v1);
    g->uv2 = vec2(u2, v2);
    g->uv3 = vec2(u3, v3);

}


// Enable texturing (pass NULL texture to disable)
void g_toggle_texturing(Graphics* g, Bitmap* tex) {

    g->tex = tex;
    if (tex == NULL || tex->width <= 0 || tex->height <= 0)
        g->pfunc = pfunc_default;
}


// Darken the screen
void g_darken(Graphics* g, int level) {

    int x, y;
    int offset;

    for (y = 0; y < g->csize.y; ++ y) {

        for (x = 0; x < g->csize.x; ++ x) {

            offset = y * g->csize.x + x;
            g->pdata[offset] 
                = g->darray[ ditherArray[level] [ x % 2 == y % 2] ] [g->pdata[offset] ];
        }
    }
}


// Set darkness tint color
// (I know right)
void g_set_darkness_color(Graphics* g, uint8 col) {

    switch (col)
    {
    case ColorBlack:
        g->darray = dpalette;
        break;
    case ColorRed:
        g->darray = redPalette;
        break;
    case ColorWhite:
        g->darray = lpalette;
        break;
    
    default:
        break;
    }
}


// Copy current screen to the buffer
void g_copy_to_buffer(Graphics* g) {

    memcpy(g->pbuffer, g->pdata, g->csize.x*g->csize.y);
}


// Fill the screen with zoomed rotated bitmap
void g_fill_zoomed_rotated(Graphics* g, Bitmap* bmp,
    float angle, float sx, float sy) {

    int x, y;
    int px, py;
    int tx, ty;
    int offset;

    // Transition
    Point tr = point(bmp->width/2, bmp->height/2);

    // Compute sine & cosine
    int s = (int) (sinf(angle) * FIXED_PREC);
    int c = (int) (cosf(angle) * FIXED_PREC);

    int scalex = (int) (sx * FIXED_PREC);
    int scaley = (int) (sy * FIXED_PREC);

    for (y = 0; y < g->csize.y; ++ y) {

        for (x = 0; x < g->csize.x; ++ x) {

            px = (x-tr.x) * scalex / FIXED_PREC;
            py = (y-tr.y) * scaley / FIXED_PREC;

            // Compute transformed pixel position
            tx = ( px * c - py * s) / FIXED_PREC;
            ty = ( px * s + py * c) / FIXED_PREC;

            tx -= tr.x;
            ty -= tr.y;

            tx = neg_mod(tx, bmp->width);
            ty = neg_mod(ty, bmp->height);

            offset = y * g->csize.x + x;
            g->pdata[offset] = bmp->data[ty*bmp->width + tx];
        }
    }
}
