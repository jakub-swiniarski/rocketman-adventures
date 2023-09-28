#ifndef STRUCTS_H
#define STRUCTS_H

#include <raylib.h>
#include <sys/types.h>

static struct{
    Image redSoldier;
    Image rocket;
    Image launcher;
    Image particleSmoke;
    Image sky;
    Image platform;
} Images;

typedef struct{
    Texture tx;
    short x,y;
    short speedX, speedY; //used for gravity and jumping
    int8_t cooldown;
    bool falling;
} Soldier;

typedef struct{
    Texture tx;
    short x,y;
    unsigned short rotation;
    short speedX,speedY;
    bool collided;
} Rocket;

typedef struct{
    Texture tx;
    short x,y;
    unsigned short rotation;
    int8_t flip; //1 means befault, -1 means flipped
} Launcher;

typedef struct{
    Texture tx;
    short x,y;
    unsigned short rotation;
    u_int8_t alpha;
    int8_t cooldownAlpha;
} Particle;

typedef struct{
    Texture tx;
    short x,y;
} Platform;

#endif
