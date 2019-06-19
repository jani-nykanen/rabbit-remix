//
// Virtual gamepad
// (c) 2019 Jani Nykänen
//


#ifndef __GAMEPAD__
#define __GAMEPAD__

#include "input.h"
#include "types.h"

#define MAX_BUTTON_COUNT 16
#define MAX_BUTTON_NAME_LENGTH 16

// Button type
typedef struct {

    int key;
    int joybutton;
    char name[MAX_BUTTON_NAME_LENGTH];

} Button;

// Gamepad type
typedef struct {

    // Reference to the input manager
    Input* input;

    // Values
    Button buttons [MAX_BUTTON_COUNT];
    int buttonCount;
    Vector2 stick;
    Vector2 delta;

} Gamepad;

// Create a gamepad
Gamepad create_gamepad(Input* inputRef);

// Add a button
int pad_add_button(Gamepad* vpad, 
    const char* name, int key, int joybutton);
// Get button state
State pad_get_button_state(Gamepad* vpad, const char* bname);

// Update
void pad_update(Gamepad* vpad);

// Parse a text file
int pad_parse_text_file(Gamepad* vpad, const char* path);

#endif // __GAMEPAD__
