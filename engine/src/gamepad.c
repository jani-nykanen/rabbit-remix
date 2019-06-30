#include "gamepad.h"

#include "wordreader.h"
#include "err.h"


// Create a gamepad
Gamepad create_gamepad(Input* inputRef) {

    Gamepad vpad;

    vpad.input = inputRef;
    vpad.buttonCount = 0;

    vpad.stick = vec2(0, 0);
    vpad.delta = vec2(0, 0);

    return vpad;
}


// Add a button
int pad_add_button(Gamepad* vpad, 
    const char* name, int key, int joybutton) {

    // Check if room
    if (vpad->buttonCount == MAX_BUTTON_COUNT) {

        err_throw_no_param("Gamepad buttons full.");
        return -1;
    }

    Button b;
    snprintf(b.name, MAX_BUTTON_NAME_LENGTH, "%s", name);
    b.key = key;
    b.joybutton = joybutton;

    vpad->buttons[vpad->buttonCount ++] = b;

    return 0;
}


// Get button state
State pad_get_button_state(Gamepad* vpad, const char* bname) {
    
    // Find a button
    int i = 0;
    Button* b = NULL;
    for(; i < vpad->buttonCount; ++ i) {

        if (strcmp(bname, vpad->buttons[i].name) == 0) {

            b = &vpad->buttons[i];
            break;
        }
    }
    if (b == NULL)
        return StateUp;


    // Check keyboard first, then joystick
    State s = input_get_key_state(vpad->input, b->key);
    if (s == StateUp)
        s = input_get_joy_state(vpad->input, b->joybutton);

    return s;
}


// Update
void pad_update(Gamepad* vpad) {

    const float EPS = 0.01f;

    Vector2 oldPos = vpad->stick;

    // First check joystick
    vpad->stick.x = vpad->input->joystick.x;
    vpad->stick.y = vpad->input->joystick.y;

    // If too small a movement, check keyboard
    if (hypotf(vpad->stick.x, vpad->stick.y) < EPS) {

        // Emulate analogue stick with arrow keys
        if (input_get_key_state(vpad->input, (int)SDL_SCANCODE_RIGHT)
            == StateDown) {
            vpad->stick.x = 1;
        }
        else if (input_get_key_state(vpad->input, (int)SDL_SCANCODE_LEFT)
            == StateDown) {
            vpad->stick.x = -1;
        }

        if (input_get_key_state(vpad->input, (int)SDL_SCANCODE_DOWN)
            == StateDown) {
            vpad->stick.y = 1;
        }
        else if (input_get_key_state(vpad->input, (int)SDL_SCANCODE_UP)
            == StateDown) {
            vpad->stick.y = -1;
        }

        // Note that we DO NOT restrict the stick
        // to B(0,1) since, for example, in platformers, that would
        // be unwanted behaviour
    }

    // Compute delta
    vpad->delta.x = vpad->stick.x - oldPos.x;
    vpad->delta.y = vpad->stick.y - oldPos.y;
}


// Parse a text file
int pad_parse_text_file(Gamepad* vpad, const char* path) {

    // Create a word reader
    WordReader* wr = create_word_reader(path);
    if (wr == NULL) {

        return -1;
    }

    // Find buttons
    char name[WR_WORD_LENGTH];
    char keystr[WR_WORD_LENGTH];
    char buttonstr[WR_WORD_LENGTH];
    int c = 0;
    int key, button;
    while(wr_read_next(wr)) {

        if (c == 0) {

            snprintf(name, WR_WORD_LENGTH, "%s", wr->word);
        }
        else if (c == 1) {

            snprintf(keystr, WR_WORD_LENGTH, "%s", wr->word);
        }
        else {

            snprintf(buttonstr, WR_WORD_LENGTH, "%s", wr->word);

            key = (int)strtol(keystr, NULL, 10);
            button = (int)strtol(buttonstr, NULL, 10);
            pad_add_button(vpad, name, key, button);

        }   
        ++ c;
        c %= 3;
    }

    // Close & destroy
    dispose_word_reader(wr);
}
