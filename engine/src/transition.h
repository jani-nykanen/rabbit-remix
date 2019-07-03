//
// Transition
// (c) 2019 Jani Nykänen
//


#ifndef __TRANSITION__
#define __TRANSITION__

#include  "graphics.h"

#include <stdbool.h>


// Mode
enum {
    FadeIn = 0,
    FadeOut = 1
};
typedef int Mode;

// Transition type
typedef struct {
    
    Mode mode;
    float timer;
    float speed;
    bool active;
    void (*callback)(void);
    uint8 fadeColor;

} Transition;


// Create a transition object
Transition create_transition_object();

// Activate
void tr_activate(Transition* tr, Mode mode, 
    float speed, void (*cb)(void), uint8 c);

// Update transition
void tr_update(Transition* tr, float tm);
// Draw transition
void tr_draw(Transition* tr, Graphics* g);


#endif // __TRANSITION__
