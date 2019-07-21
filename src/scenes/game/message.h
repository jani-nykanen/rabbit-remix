//
// A flying message
//

#ifndef __MESSAGE__
#define __MESSAGE__

#include <engine/graphics.h>

#define MSG_LENGTH_MAX 16

// Message type
typedef struct {
    
    // Does exist
    bool exist;

    // Message text
    char text [MSG_LENGTH_MAX];

    // Position
    Vector2 pos;

    // Timer
    float timer;
    // Speed (vertical)
    float speed;
    // Phase
    int phase;

} Message;

// Create a message object
Message create_message();

// Activate a message
void msg_activate(Message* m, Vector2 pos, float speed, 
    const char* text, int param);

// Update a message
void msg_update(Message* m, float tm);

// Draw a message
void msg_draw(Message* m, Graphics* g, Bitmap* bmp);

// Get the next available message in the array
Message* msg_get_next(Message* msgs, int len);

// Create a score message (with defaul speed)
void msg_create_score_message(Message* msgs, 
    int len, int score, Vector2 pos);

#endif // __MESSAGE__
