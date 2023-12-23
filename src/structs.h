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
    Image bg[9];
} Images;

typedef struct{
    Texture tx;
    int x,y;
    int speedX, speedY; //used for gravity and jumping
    int cooldown;
    bool falling;
    int pickup, pickupActive;
    float slowfall; //1 means no slowfall
    int critBoost;
    int hp;
} Soldier;

typedef struct{
    Texture tx;
    int x,y;
    int rotation;
    int speedX,speedY;
    bool collided,shouldExplode,isFree;
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
    bool isFree;
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
} HealthPack;

typedef struct{
    Texture tx;
    int x,y;
    char text[16];
    Color textColor;
} HUD;

typedef struct{
    Texture tx[2];
    int x,y;
    char text[12];
    int state; //0 - normal, 1 - hover
} Button;

#endif
