#ifndef STRUCTS_H
#define STRUCTS_H

#include "globals.h"
#include <raylib.h>
#include <stdint.h>

static struct{
    //soldier
    Image red_soldier;
    Image rocket;
    Image launcher;
    Image parachute; 
    
    //world
    Image platform;
    
    //pickups
    Image parachute_pickup;
    Image crit_pickup;
    Image health_pack;
    
    //hud
    Image hud; 
    Image button[2];

    //visuals
    Image particle_smoke;
 
    //backgrounds
    Image bg[9];
} Images;

typedef struct{
    Texture tx;
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
    Texture tx;
    int x,y;
    int rotation;
    int speed_x,speed_y;
    bool collided,should_explode,is_free;
} Rocket;

typedef struct{
    Texture tx;
    int x,y;
    int rotation;
    int flip; //1 means befault, -1 means flipped
    Color color;
} Launcher;

typedef struct{
    Texture tx;
    int x,y;
    int rotation;
    int alpha;
    bool is_free;
} Particle;

typedef struct{
    Texture tx;
    int x,y;
} Platform;

typedef struct{
    Texture tx;
    Texture txs[2];
    int x,y;
    int id; //1 - parachute, 2 - crits
} Pickup;

typedef struct{
    Texture tx;
    int x,y;
} Health_pack;

typedef struct{
    Texture tx;
    int x,y;
    char text[16];
    Color text_color;
} HUD;

typedef struct{
    Texture tx[2];
    int x,y;
    char text[12];
    int state; //0 - normal, 1 - hover
} Button;

#endif
