#ifndef STRUCTS_H
#define STRUCTS_H

#include <raylib.h>

#include "globals.h"

static struct {
    //soldier
    Texture red_soldier[6];
    Texture red_soldier_jumping;
    Texture rocket;
    Texture launcher;
    Texture parachute; 
    
    //world
    Texture platform;
    
    //pickups
    Texture pickup[NUM_PICKUP];
    Texture health_pack;

    //hud
    Texture hud; 
    Texture button[2];

    //visuals
    Texture particle_smoke;
 
    //backgrounds
    Texture bg[NUM_BG];
} TextureHolder;

typedef struct {
    Texture *tx;
    int x, y;
    int speed_x, speed_y; /* used for gravity and jumping */
    float rl_cooldown;
    bool falling;
    int pickup, pickup_active;
    float slow_fall; /* 1 means no slow fall */
    int crit_boost;
    int hp;
    int flip; /* 1 means befault, -1 means flipped */
    Color color;
    int state;
    float anim_cooldown;
    int frame;
} Soldier;

typedef struct Rocket Rocket;
struct Rocket {
    Texture *tx;
    int x, y;
    int rotation;
    int speed_x, speed_y;
    bool collided, should_explode;
    Rocket *next;
};

typedef struct {
    Texture *tx;
    int x, y;
    int rotation;
} Launcher;

typedef struct {
    Texture *tx;
    int y;
} Background;

typedef struct Particle Particle;
struct Particle {
    Texture *tx;
    int x, y;
    int rotation;
    int alpha;
    Particle *next;
};

typedef struct {
    Texture *tx;
    int rotation;
} Parachute;

typedef struct {
    Texture *tx;
    int x, y;
} Platform;

typedef struct {
    Texture *tx;
    int x, y;
    int id;
} Pickup;

typedef struct {
    Texture *tx;
    int x, y;
} HealthPack;

typedef struct {
    Texture *tx;
    int x, y;
    char text[16];
    Color text_color;
} HUD;

typedef struct {
    Texture *tx;
    int x, y;
    char text[12];
    int state;
} Button;

#endif
