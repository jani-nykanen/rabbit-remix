#include "message.h"

#include <math.h>

// Constants
static const float MSG_TIME[] = {
    15.0f, 30.0f,
};


// Create a message object
Message create_message() {

    Message m;
    
    m.exist = false;

    return m;
}


// Activate a message
void msg_activate(Message* m, Vector2 pos, float speed, 
    const char* text, int param) {

    // Copy message
    snprintf(m->text, MSG_LENGTH_MAX, "%s%d", text, param);

    m->speed = speed;
    m->timer = MSG_TIME[0];
    m->phase = 0;
    m->pos = pos;

    m->exist = true;

}


// Update a message
void msg_update(Message* m, float tm) {

    if (!m->exist) return;

    // Update timer
    if ((m->timer -= 1.0f * tm) <= 0.0f) {

        if (m->phase == 1) {

            m->exist = false;
            return;
        }
        else {

            m->timer = MSG_TIME[++ m->phase];
        }
    }

    // Phase specific behavior
    if (m->phase == 0) {

        m->pos.y += m->speed * tm;
    }
}


// Draw a message
void msg_draw(Message* m, Graphics* g, Bitmap* bmp) {

    const int XOFF = -1;

    if (!m->exist) return;

    g_draw_text(g, bmp, m->text,
        (int)roundf(m->pos.x), (int)roundf(m->pos.y),
        XOFF, 0, true);
}


// Get the next available message in the array
Message* msg_get_next(Message* msgs, int len) {

    int i;
    for (i = 0; i < len; ++ i) {

        if (msgs[i].exist == false) {

            return &msgs[i];
        }
    }
    return NULL;
}


// Create a score message (with defaul speed)
void msg_create_score_message(Message* msgs, 
    int len, int score, Vector2 pos) {

    const float DEFAULT_SPEED = -2.5f;

    Message* m = msg_get_next(msgs, len);
    if (m == NULL) return;

    msg_activate(m, pos, DEFAULT_SPEED, "+", score);
}
