#include "enemy.h"

// Global bitmaps
static Bitmap* bmpEnemy;


// Initialize global enemy content
void init_global_enemies(AssetManager* a) {

    // Get bitmaps
    bmpEnemy = (Bitmap*)assets_get(a, "enemy");
}


// Create an enemy
Enemy create_enemy(){

    Enemy e;
    e.exist = false;

    return e;
}


// Activate an enemy
void enemy_activate(Enemy* e, Vector2 pos, int id){
    
    e->spr = create_sprite(48, 48);
    e->pos = pos;
    e->startPos = pos;
    e->id = id;

    e->exist = true;
}


// Update an enemy
void enemy_update(Enemy* e, float globalSpeed, float tm){
    
    if (!e->exist) return;

    // TEMP
    e->pos.x -= globalSpeed * tm;

    // See if has "gone too far"
    if (e->pos.x + e->spr.width/2 < 0) {

        e->exist = false;
    }
}


// Bullet-enemy collision
void enemy_bullet_collision(
    Enemy* e, Bullet* b, Coin* coins, int coinLen){
        
    if (!e->exist || !b->exist || b->dying) 
        return;
}


// Player-enemy collision
void enemy_player_collision(Enemy* e, Player* pl, 
    Coin* coins, int coinLen) {

    if (!e->exist || pl->dying || pl->respawnTimer > 0.0f ||
        pl->invincibilityTimer > 0.0f) 
        return;   
}


// Draw an enemy
void enemy_draw(Enemy* e, Graphics* g) {
    
    const int DVALUE = 8;

    if (!e->exist) return;

    int x = (int) roundf(e->pos.x);
    int y = (int) roundf(e->pos.y);

    g_set_pixel_function(g, PixelFunctionDarken, DVALUE, 0);

    spr_draw(&e->spr, g, bmpEnemy, 
        x - e->spr.width/2, y - e->spr.height/2, false);

    g_set_pixel_function(g, PixelFunctionDefault, 0, 0);
}
