//
// Input manager
// (c) 2019 Jani Nykänen
//


#ifndef __INPUT__
#define __INPUT__

#include "types.h"

#include <SDL2/SDL.h>
#include <stdbool.h>

// Array sizes
#define KEY_ARRAY_SIZE 256
#define JOY_ARRAY_SIZE 16

// States
enum {

    StateUp = 0,
    StateDown = 1,
    StateReleased = 2,
    StatePressed = 3,

    AxisX = 0,
    AxisY = 1,
};
typedef int State;

// Input activity type
enum {

    ActivityKeyboard = 0,
    ActivityJoystick = 1,
};
typedef int Activity;

// Input manager
typedef struct {

    // State arrays
    int keyStates [KEY_ARRAY_SIZE];
    int joyStates [JOY_ARRAY_SIZE];

    // Joystick stick state
    Vector2 joystick;
    // Is joystick active
    bool joyactive;

    // Latest activity
    Activity activity;

} InputManager;
typedef InputManager Input; // Laziness

// Create input manager
Input create_input_manager();

// Key events
void event_key_down(Input* input, int key);
void event_key_up(Input* input, int key);

// Joystick button events
void event_joy_down(Input* input, int joy);
void event_joy_up(Input* input, int joy);

// Joystick stick event
void event_joy_move(Input* input, float value, int axis);
void event_joy_move_axes(Input* input, float x, float y);

// Get a key state
State input_get_key_state(Input* input, int key);
// Get a joystick button state
State input_get_joy_state(Input* input, int button);

// Update input
void input_update(Input* input);

#endif // __INPUT__
