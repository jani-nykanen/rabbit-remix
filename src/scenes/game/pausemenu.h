//
// Pause menu
// (c) 2019 Jani Nykänen
//

#ifndef __PAUSE_MENU__
#define __PAUSE_MENU__

#include "../../menu.h"

// Pause menu type
typedef struct
{
    Menu menu;
    bool active;
    bool bufferCopied;

} PauseMenu;

// Create a pause menu
PauseMenu create_pause_menu();

// Activate pause menu
void pause_activate(PauseMenu* pm);

// Update pause menu
void pause_update(PauseMenu* pm, EventManager* evMan);

// Draw pause menu
void pause_draw(PauseMenu* pm, Graphics* g);

#endif // __PAUSE_MENU__
