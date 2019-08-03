#include "menu.h"

#include <engine/mathext.h>

#include <stdio.h>

// Global bitmaps
static Bitmap* bmpFont;
// Global samples
static Sample* sChoose;
static Sample* sSelect;


// Initialize global menu content
void init_global_menus(AssetManager* a) {

    // Get bitmaps
    bmpFont = (Bitmap*)assets_get(a, "font");

    // Get samples
    sChoose = (Sample*)assets_get(a, "choose");
    sSelect = (Sample*)assets_get(a, "select");
}


// Create a menu
Menu create_menu() {

    Menu m;
    m.buttonCount = 0;
    m.cpos = 0;

    return m;
}


// Add a button
void menu_add_button(Menu* m, 
    void (*cb)(EventManager* evMan), 
    const char* text) {

    if (m->buttonCount >= BUTTON_MAX)  
        return;    

    MenuButton b;
    snprintf(b.text, BUTTON_TEXT_LENGTH, " %s", text);
    b.cb = cb;

    m->buttons[m->buttonCount ++] = b;
}


// Update menu
void menu_update(Menu* m, EventManager* evMan) {
    
    const float EPS = 0.1f;

    int opos = m->cpos;

    // Check vertical movement
    float stickDelta = evMan->vpad->delta.y;
    float stickPos = evMan->vpad->stick.y;

    if (stickPos > EPS && stickDelta > EPS) {

        ++ m->cpos;
    }
    else if(stickPos < -EPS && stickDelta < -EPS) {

        -- m->cpos;
    }
    m->cpos = neg_mod(m->cpos, m->buttonCount);

    // Place changed, play sound
    if (m->cpos != opos) {

        audio_play_sample(evMan->audio, sSelect, 0.70f, 0);
    }

    // Check enter or fire1
    // Wait for enter or jump button
    if (pad_get_button_state(evMan->vpad, "fire1") == StatePressed ||
        pad_get_button_state(evMan->vpad, "start") == StatePressed) {

        if (m->buttons[m->cpos].cb != NULL) {

            m->buttons[m->cpos].cb(evMan);
        }

        audio_play_sample(evMan->audio, sChoose, 0.70f, 0);
    }
}


// draw menu
void menu_draw(Menu* m, Graphics* g, int x, int y) {

    const int XOFF = 0;
    const int YOFF = 10;

    int i;
    for (i = 0; i < m->buttonCount; ++ i) {

        m->buttons[i].text[0] = m->cpos == i ? '\6' : ' ';
        g_draw_text(g, bmpFont, m->buttons[i].text,
            x, y + i * YOFF, XOFF, 0, false);
    }
}
