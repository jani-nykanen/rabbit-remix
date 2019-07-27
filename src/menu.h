// 
// A generic menu
// (c) 2019 Jani Nykänen
//

#ifndef __MENU__
#define __MENU__

#include <engine/graphics.h>
#include <engine/assets.h>
#include <engine/eventmanager.h>

#define BUTTON_TEXT_LENGTH 16
#define BUTTON_MAX 16

// Initialize global menu content
void init_global_menus(AssetManager* a);

// Button type
typedef struct {

    char text [BUTTON_TEXT_LENGTH];
    void (*cb) (EventManager* evMan);

} MenuButton;

// Menu type
typedef struct {

    MenuButton buttons [BUTTON_MAX];
    int buttonCount;

    // Cursor position
    int cpos;

} Menu;

// Create a menu
Menu create_menu();

// Add a button
void menu_add_button(Menu* m, 
    void (*cb)(EventManager* evMan), 
    const char* text);

// Update menu
void menu_update(Menu* m, EventManager* evMan);

// draw menu
void menu_draw(Menu* m, Graphics* g, int x, int y);

#endif // __MENU__
