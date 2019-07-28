#include "input.h"

#include <math.h>


// State array event
static void event_state_array(int arr[], int len, int index, int state) {

    if (index < 0 || index >= len || state-2 == arr[index])
        return;
        
    arr[index] = state;
}


// Update a state array
static void update_state_array(int arr[], int len) {

    int i = 0;
    for(; i < len; ++ i) {

        if (arr[i] == StatePressed)
            arr[i] = StateDown;
        
        else if (arr[i] == StateReleased)
            arr[i] = StateUp;
    }
}


// Get an array state
static int get_array_state(int arr[], int len, int index) {

    if (index < 0 || index >= len)
        return StateUp;

    return arr[index];
}


// Create input manager
Input create_input_manager() {

    Input input;
    int i = 0;

    // Initialize arrays
    for(i = 0; i < KEY_ARRAY_SIZE; ++ i) {

        input.keyStates[i] = StateUp;
    }
    for(i = 0; i < JOY_ARRAY_SIZE; ++ i) {

        input.joyStates[i] = StateUp;
    }

    input.joystick = vec2(0, 0);
    input.activity = ActivityKeyboard;

    // Check joystick
    input.joyactive = SDL_JoystickOpen(0) != NULL;

    return input;
}


// Key events
void event_key_down(Input* input, int key) {

    event_state_array(input->keyStates, 
        KEY_ARRAY_SIZE, key, StatePressed);

    input->activity = ActivityKeyboard;
}
void event_key_up(Input* input, int key){

    event_state_array(input->keyStates, 
        KEY_ARRAY_SIZE, key, StateReleased);

    input->activity = ActivityKeyboard;
}


// Joystick button events
void event_joy_down(Input* input, int joy) {

    if (!input->joyactive) return;

    event_state_array(input->joyStates,
        JOY_ARRAY_SIZE, joy, StatePressed);

    input->activity = ActivityJoystick;
}
void event_joy_up(Input* input, int joy) {

    if (!input->joyactive) return;

    event_state_array(input->joyStates,
        JOY_ARRAY_SIZE, joy, StateReleased);

    input->activity = ActivityJoystick;
}


// Joystick stick event
void event_joy_move(Input* input, float value, int axis) {

    const float EPS = 0.1f;

    if (!input->joyactive) return;

    if (axis == AxisX)
        input->joystick.x = value;
    else
        input->joystick.y = value;

    if (hypotf(input->joystick.x, input->joystick.y) > EPS)
        input->activity = ActivityJoystick;
}
void event_joy_move_axes(Input* input, float x, float y) {

    const float EPS = 0.1f;

    if (!input->joyactive) return;

    input->joystick.x = x;
    input->joystick.y = y;

    input->activity = ActivityJoystick;

    if (hypotf(input->joystick.x, input->joystick.y) > EPS)
        input->activity = ActivityJoystick;
}


// Get a key state
State input_get_key_state(Input* input, int key) {

    get_array_state(input->keyStates, KEY_ARRAY_SIZE, key);
}


// Get a joystick button state
State input_get_joy_state(Input* input, int button) {

    get_array_state(input->joyStates, JOY_ARRAY_SIZE, button);
}


// Update input
void input_update(Input* input) {

    // Update state arrays
    update_state_array(input->keyStates, KEY_ARRAY_SIZE);
    if (input->joyactive)
        update_state_array(input->joyStates, JOY_ARRAY_SIZE);
}
