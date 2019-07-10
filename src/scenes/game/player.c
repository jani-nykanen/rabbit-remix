#include "player.h"

// Shared pointers to bitmaps
static Bitmap* bmpBunny;


// Init global data
void init_global_player(AssetManager* a) {

    bmpBunny = (Bitmap*)assets_get(a, "bunny");
}


// Create a player
Player create_player(int x, int y) {

    Player pl;

    // Set position & speeds
    pl.pos = vec2((float)x, (float)y);
    pl.speed = vec2(0, 0);
    pl.target = vec2(0, 0);

    // Set other initial values
    pl.jumpTimer = 0.0f;
    pl.doubleJump = false;

    // Create sprite
    pl.spr = create_sprite(48, 48);

    return pl;
}


// Update player
void pl_update(Player* pl, EventManager* evMan, float tm) {

}


// Draw player
void pl_draw(Player* pl, Graphics* g) {

    // Draw sprite
    spr_draw(&pl->spr, g, bmpBunny, 
        (int)roundf(pl->pos.x-24), 
        (int)roundf(pl->pos.y-48), false);
}
