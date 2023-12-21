#ifndef CONFIG_H
#define CONFIG_H

#include <raylib.h>

//screen resolution
static const int SCREENWIDTH=1920;
static const int SCREENHEIGHT=1080;

//frames per second
static const int FPS=60;

//sound
static int volume=100;
static bool muted=0;

//HUD
static const Color TEXTCOLOR[3]={RED,WHITE,GREEN}; //low, normal, high

//keyboard controls
static const KeyboardKey MOVELEFT=KEY_A;
static const KeyboardKey MOVERIGHT=KEY_D;
static const KeyboardKey JUMP=KEY_SPACE;
static const KeyboardKey SHOOT_ALT=KEY_R;
static const KeyboardKey USEPICKUP=KEY_Q;
static const KeyboardKey MUTE=KEY_F1;
static const KeyboardKey VOLUP=KEY_F3;
static const KeyboardKey VOLDOWN=KEY_F2;

//mouse controls
static const MouseButton SHOOT=MOUSE_LEFT_BUTTON;

//gameplay
static const int NUM_PLATFORMS=10;
static const int NUM_HEALTHPACKS=2;

#endif 
