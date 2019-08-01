#include "settings.h"

#include <engine/mathext.h>
#include <engine/eventmanager.h>

#include <math.h>
#include <stdlib.h>

// Bitmaps
static Bitmap* bmpFont;
static Bitmap* bmpSettings;
static Bitmap* bmpCog;

// Was the previous scene the game scene
static bool prevSceneGame;

// Wave timer
static float waveTimer;
// Cog angle
static float cogAngle;

// Cursor position
static int cpos;
// Framerate
static int frameRate;

// Audio volumes
static int musicVol;
static int sfxVol;


// Change back to game
static void change_back(void* e) {

    ev_change_scene((EventManager*)e, 
        "game", NULL);

    Transition* tr = ((EventManager*)e)->tr;
    tr->effect = EffectCircle;
    tr->speed = 1.0f;
}
// Go to the previous scene
static void go_back(void* e) {

    ev_change_scene((EventManager*)e, 
        prevSceneGame ? "game" : "title", 
        (void*)(size_t)1);
}


// Initialize
static int settings_init(void* e) {

    return 0;
}


// On load
static int settings_on_load(AssetManager* a) { 

    // Get bitmaps
    bmpFont = (Bitmap*)assets_get(a, "font");
    bmpSettings = (Bitmap*)assets_get(a, "settings");
    bmpCog = (Bitmap*)assets_get(a, "cog");

    // Set initials
    waveTimer = 0.0f;
    cogAngle = 0.0f;
    cpos = 4;
    musicVol = 0;
    sfxVol = 0;
}


// Update volumes
static void update_volumes(EventManager* evMan) {

    const float EPS = 0.1f;
    const int AUDIO_JUMP = 10;

    if (cpos > 1) return;

    AudioPlayer* audio = evMan->audio;

    float delta = evMan->vpad->delta.x;
    float stick = evMan->vpad->stick.x;

    int dir;

    if (fabsf(stick) > EPS && delta/stick > 0.0f) {

        dir = stick > 0.0f ? 1 : -1;

        if (cpos == 0) {

            audio_change_sfx_volume(audio, 
                audio->sfxVolume + dir * AUDIO_JUMP);
        }
        else {

            audio_change_music_volume(audio, 
                audio->musicVolume + dir * AUDIO_JUMP);
        }
    }
}


// Update
static void settings_update(void* e, float tm) {

    const float WAVE_SPEED = 0.1f;
    const float COG_SPEED = 0.05f;
    const float EPS = 0.1f;

    EventManager* evMan = (EventManager*)e;
    AudioPlayer* audio = evMan->audio;

    // Get stuff
    frameRate = ev_get_framerate(evMan);
    sfxVol = audio->sfxVolume;
    musicVol = audio->musicVolume;

    if (evMan->tr->active) return;

    // Update waves & cog angle
    waveTimer += WAVE_SPEED * tm;
    cogAngle += COG_SPEED * tm;
    waveTimer = fmodf(waveTimer, M_PI * 2);
    cogAngle = fmodf(cogAngle, M_PI * 2);

    // Check vertical movement
    float stickDelta = evMan->vpad->delta.y;
    float stickPos = evMan->vpad->stick.y;

    if (stickPos > EPS && stickDelta > EPS) {

        ++ cpos;
    }
    else if(stickPos < -EPS && stickDelta < -EPS) {

        -- cpos;
    }
    cpos = neg_mod(cpos, 5);

    // Check enter or fire1
    // Wait for enter or jump button
    if (pad_get_button_state(evMan->vpad, "fire1") == StatePressed ||
        pad_get_button_state(evMan->vpad, "start") == StatePressed) {

        switch (cpos)
        {

        // Full screen
        case 2:
            ev_toggle_fullscreen(evMan);
            break;

        // Change frame rate
        case 3:

            ev_set_framerate(evMan, frameRate != 60 ? 60 : 30);
            break;

        // Back
        case 4:
            tr_activate(evMan->tr, FadeIn, EffectFade, 2.0f,
                go_back, 0);
            break;
        
        default:
            break;
        }
    }

    // Update volumes
    update_volumes(evMan);
}


// Draw a cog
static void draw_cog(Graphics* g, int x, int y, 
    float angle, float r) {

    float step = M_PI*2.0f / 4.0f;

    int x1 = x + (int)(cosf(angle) * r);
    int y1 = y + (int)(sinf(angle) * r);

    int x2 = x + (int)(cosf(angle + step) * r);
    int y2 = y + (int)(sinf(angle + step) * r);

    int x3 = x + (int)(cosf(angle + 2*step) * r);
    int y3 = y + (int)(sinf(angle + 2*step) * r);

    int x4 = x + (int)(cosf(angle + 3*step) * r);
    int y4 = y + (int)(sinf(angle + 3*step) * r);

    g_toggle_texturing(g, bmpCog);

    g_set_uv_coords(g, 0, 0, 1, 0, 1, 1);
    g_draw_triangle(g, x1, y1, x2, y2, x3, y3, 0);

    g_set_uv_coords(g, 1, 1, 0, 1, 0, 0);
    g_draw_triangle(g, x3, y3, x4, y4, x1, y1, 0);

    g_toggle_texturing(g, NULL);
}


// Draw title
static void draw_title(Graphics* g) {

    const int POS_Y = 24;
    const int XOFF = -3;
    const float PERIOD = M_PI*2.0f / 8.0f;
    const float AMPLITUDE = 4.0f;

    const int SHADOW_DARKEN = 4;
    const int SHADOW_OFF_X = 1;
    const int SHADOW_OFF_Y = 2;

    int i;
    int midx = g->csize.x / 2;
    int dx = midx - (8+1) * (16+XOFF)/2;
    int step = (16+XOFF);

    // Shadows
    g_set_pixel_function(g, PixelFunctionDarken, SHADOW_DARKEN, 0);
    for (i = 0; i < 8; ++ i) {

        g_draw_bitmap_region(g, bmpSettings, 
            i*16, 0, 16, 16, 
            dx + step*i + SHADOW_OFF_X,
            POS_Y + 
                (int)(sinf(waveTimer + PERIOD*i) * AMPLITUDE) +
                SHADOW_OFF_Y, 
            false);
    }

    // Base
    g_set_pixel_function(g, PixelFunctionDefault, 0, 0);
    for (i = 0; i < 8; ++ i) {

        g_draw_bitmap_region(g, bmpSettings, 
            i*16, 0, 16, 16, 
            dx + step*i,
            POS_Y + (int)(sinf(waveTimer + PERIOD*i) * AMPLITUDE), 
            false);
    }
}


// Draw menu
static void draw_menu(Graphics* g) {

    const int POS_X = 56;
    const int START_Y = 72;
    const int Y_OFF = 16;

    char buf[32];

    // SFX
    snprintf(buf, 32, "SFX VOLUME:   %d", sfxVol);
    g_draw_text(g, bmpFont, buf, 
        POS_X, START_Y, 0, 0, false);

    // Music
    snprintf(buf, 32, "MUSIC VOLUME: %d", musicVol);
    g_draw_text(g, bmpFont, buf, 
        POS_X, START_Y + Y_OFF, 0, 0, false);

    // Toggle fullscreen
    g_draw_text(g, bmpFont, "TOGGLE FULLSCREEN", 
        POS_X, START_Y + Y_OFF*2, 0, 0, false);

    // Framerate
    snprintf(buf, 32, "FRAMERATE: %d FPS", frameRate);
    g_draw_text(g, bmpFont, buf, 
        POS_X, START_Y + Y_OFF*3, 0, 0, false);

    // Back
    g_draw_text(g, bmpFont, "BACK", 
        POS_X, START_Y + Y_OFF*5, -1, 0, false);  

    // Draw cursor
    g_draw_text(g, bmpFont, "\6", 
        POS_X - 8, 
        START_Y + Y_OFF * (cpos + cpos/4), 
        0, 0, false);
}


// Draw cogs
static void draw_cogs(Graphics* g) {

    const float RADIUS = 48;

    draw_cog(g, 0, 0, cogAngle, RADIUS);
    draw_cog(g, g->csize.x, 0, -cogAngle, RADIUS);
    draw_cog(g, g->csize.x, g->csize.y, -cogAngle, RADIUS);
    draw_cog(g, 0, g->csize.y, cogAngle, RADIUS);
}


// Draw
static void settings_draw(Graphics* g) {
        
    g_clear_screen(g, 0b01010011);

    // Draw cogs
    draw_cogs(g);
    // Draw title
    draw_title(g);
    // Draw menu
    draw_menu(g);
}


// Dispose
static void settings_dispose() {

    // ...
}


// Change
static void settings_on_change(void* param) {

    prevSceneGame = (bool)(size_t)param;
    cpos = 4;
}


// Get the settings scene
Scene settings_get_scene() {

    Scene s = (Scene) {
        settings_init,
        settings_on_load,
        settings_update,
        settings_draw,
        settings_dispose,
        settings_on_change
    };

    return s;
}
