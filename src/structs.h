#ifndef STRUCTS_H
#define STRUCTS_H

#include "globals.h"
#include <raylib.h>
#include <stdint.h>

static struct{
    Image redSoldier;
    Image rocket;
    Image launcher;
    Image particleSmoke;
    Image backgrounds[2];
    Image platform;
    Image parachutePickup;
    Image parachute;
    Image critPickup;
    Image hud;
    Image healthPack;
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
    char text[5];
    Color textColor;
} HUD;

#endif
