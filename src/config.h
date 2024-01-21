#ifndef CONFIG_H
#define CONFIG_H

#include <raylib.h>

//screen resolution
static const int SCREEN_WIDTH=1920;
static const int SCREEN_HEIGHT=1080;

//frames per second
static const int FPS=60;

//sound
static float VOLUME=1;
static bool MUTED=0;

//HUD
static const Color TEXT_COLOR[3]={RED,WHITE,GREEN}; //low, normal, high

//keyboard controls
static const KeyboardKey MOVE_LEFT=KEY_A;
static const KeyboardKey MOVE_RIGHT=KEY_D;
static const KeyboardKey JUMP=KEY_SPACE;
static const KeyboardKey SHOOT_ALT=KEY_R;
static const KeyboardKey USE_PICKUP=KEY_Q;
static const KeyboardKey MUTE=KEY_F1;
static const KeyboardKey VOL_UP=KEY_F3;
static const KeyboardKey VOL_DOWN=KEY_F2;

//mouse controls
static const MouseButton SHOOT=MOUSE_LEFT_BUTTON;

//gameplay
static const int NUM_PLATFORMS=10;
static const int NUM_HEALTH_PACKS=2;

#endif 
