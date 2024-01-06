#ifndef STRUCTS_H
#define STRUCTS_H

#include "globals.h"
#include <raylib.h>
#include <stdint.h>

static struct{
    //soldier
    Texture red_soldier;
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

typedef struct{
    Texture *tx;
    int x,y;
    int speed_x, speed_y; //used for gravity and jumping
    int cooldown;
    bool falling;
    int pickup, pickup_active;
    float slow_fall; //1 means no slow fall
    int crit_boost;
    int hp;
} Soldier;

typedef struct{
    Texture *tx;
    int x,y;
    int rotation;
    int speed_x,speed_y;
    bool collided,should_explode,is_free;
} Rocket;

typedef struct{
    Texture *tx;
    int x,y;
    int rotation;
    int flip; //1 means befault, -1 means flipped
    Color color;
} Launcher;

typedef struct{
    Texture *tx;
    int y;
} Background;

typedef struct{
    Texture *tx;
    int x,y;
    int rotation;
    int alpha;
    bool is_free;
} Particle;

typedef struct{
    Texture *tx;
    int rotation;
} Parachute;

typedef struct{
    Texture *tx;
    int x,y;
} Platform;

typedef struct{
    Texture *tx;
    int x,y;
    int id; //1 - parachute, 2 - crits
} Pickup;

typedef struct{
    Texture *tx;
    int x,y;
} Health_pack;

typedef struct{
    Texture *tx;
    int x,y;
    char text[16];
    Color text_color;
} HUD;

typedef struct{
    Texture *tx;
    int x,y;
    char text[12];
    int state; //0 - normal, 1 - hover
} Button;

#endif
