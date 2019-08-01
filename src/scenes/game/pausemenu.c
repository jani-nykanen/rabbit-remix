#include "pausemenu.h"

#include "game.h"

#include <stdlib.h>

// A reference to self
static PauseMenu* self;


// Go to the title screen
static void go_to_title(void* e) {

    self->active = false;
    ev_change_scene((EventManager*)e, "title", NULL);
}
// Restart
static void restart(void* e) {

    self->active = false;
    ev_change_scene((EventManager*)e, "game", NULL);
}
// Change scene to settings
static void go_to_settings(void* e) {

    ev_change_scene((EventManager*)e, 
        "settings", (void*)(size_t)true);
}


// Button callbacks
static void cb_resume(EventManager* evMan) {

    if (self == NULL) return;

    self->active = false;
}
static void cb_restart(EventManager* evMan) {

    tr_activate(evMan->tr, FadeIn, EffectFade,
        2.0f, restart, 0);
}
static void cb_settings(EventManager* evMan) {

    tr_activate(evMan->tr, FadeIn, EffectFade,
        2.0f, go_to_settings, 0);
}
static void cb_quit(EventManager* evMan) {

    tr_activate(evMan->tr, FadeIn, EffectFade,
        2.0f, go_to_title, 0);
}


// Create a pause menu
PauseMenu create_pause_menu() {

    PauseMenu pm;

    pm.active = false;
    pm.bufferCopied = false;

    pm.menu = create_menu();

    // Add buttons
    menu_add_button(&pm.menu, cb_resume, "RESUME");
    menu_add_button(&pm.menu, cb_restart, "RESTART");
    menu_add_button(&pm.menu, cb_settings, "SETTINGS");
    menu_add_button(&pm.menu, cb_quit, "QUIT");
    
    return pm;
}


// Activate pause menu
void pause_activate(PauseMenu* pm) {

    self = pm;

    pm->active = true;
    pm->bufferCopied = false;
    pm->menu.cpos = 0;
}


// Update pause menu
void pause_update(PauseMenu* pm, EventManager* evMan) {

    if (!pm->active) return;

    // Update menu
    menu_update(&pm->menu, evMan);

    // Disable if esc pressed
    if (pad_get_button_state(evMan->vpad, "cancel") == StatePressed) {

        pm->active = false;
    }
}


// Draw pause menu
void pause_draw(PauseMenu* pm, Graphics* g) {

    const int DVALUE_BG = 5;
    const int XOFF = -36;
    const int YOFF = -20;

    if (!pm->active) return;

    // Copy buffer
    if (!pm->bufferCopied) {

        g_copy_to_buffer(g);
        pm->bufferCopied = true;
    }

    // Draw buffer as a background
    g_draw_bitmap_fast(g, &g->bufferCopy, 0, 0);
    // Darken
    g_darken(g, DVALUE_BG);

    int x = g->csize.x/2 + XOFF;
    int y = g->csize.y/2 + YOFF;

    // Draw menu    
    menu_draw(&pm->menu, g, x, y);

    // Draw guide
    game_draw_guide(g, true);
}
