#ifndef STRUCTS_H
#define STRUCTS_H

#include "globals.h"
#include <raylib.h>
#include <stdint.h>

static struct{
    //soldier
    Image redSoldier;
    Image rocket;
    Image launcher;
    Image parachute; 
    
    //world
    Image platform;
    
    //pickups
    Image parachutePickup;
    Image critPickup;
    Image healthPack;
    
    //hud
    Image hud; 
    Image button[2];

    //visuals
    Image particleSmoke;
 
    //backgrounds
    Image bgForest;
    Image bgSky[3];     
    Image bgSkyStars;
    Image bgStars;
    Image bgStarsSpace;
    Image bgSpace[2];
} Images;

typedef struct{
    Texture tx;
    short x,y;
    short speedX, speedY; //used for gravity and jumping
    i8 cooldown;
    bool falling;
    ui8 pickup, pickupActive;
    float slowfall; //1 means no slowfall
    ui8 critBoost;
    short hp;
} Soldier;

typedef struct{
    Texture tx;
    short x,y;
    us rotation;
    short speedX,speedY;
    bool collided;
    bool shouldExplode;
} Rocket;

typedef struct{
    Texture tx;
    short x,y;
    us rotation;
    i8 flip; //1 means befault, -1 means flipped
    Color color;
} Launcher;

typedef struct{
    Texture tx;
    short x,y;
    us rotation;
    ui8 alpha;
} Particle;

typedef struct{
    Texture tx;
    short x,y;
} Platform;

typedef struct{
    Texture tx;
    Texture txs[2];
    short x,y;
    ui8 id; //1 - parachute, 2 -crits
} Pickup;

typedef struct{
    Texture tx;
    short x,y;
} HealthPack;

typedef struct{
    Texture tx;
    short x,y;
    char text[16];
    Color textColor;
} HUD;

typedef struct{
    Texture tx[2];
    short x,y;
    char text[12];
    ui8 state; //0 - normal, 1 - hover
} Button;

#endif
