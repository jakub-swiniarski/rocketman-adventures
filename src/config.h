#ifndef CONFIG_H
#define CONFIG_H

#include "globals.h"
#include <raylib.h>

//screen resolution
static const us SCREENWIDTH=1920;
static const us SCREENHEIGHT=1080;

//frames per second
static const us FPS=60;

//HUD
static const Color TEXTCOLOR[3]={RED,WHITE,GREEN}; //low, normal, high

//keyboard controls
static const KeyboardKey MOVELEFT=KEY_A;
static const KeyboardKey MOVERIGHT=KEY_D;
static const KeyboardKey JUMP=KEY_SPACE;
static const KeyboardKey SHOOT_ALT=KEY_R;
static const KeyboardKey USEPICKUP=KEY_Q;

//mouse controls
static const MouseButton SHOOT=MOUSE_LEFT_BUTTON;

#endif 
