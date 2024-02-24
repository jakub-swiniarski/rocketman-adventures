#ifndef CONFIG_H
#define CONFIG_H

#include <raylib.h>

/* screen resolution */
static const int SCREEN_WIDTH = 1920;
static const int SCREEN_HEIGHT = 1080;

/* frames per second */
static const int FPS = 60;

/* sound */
static float volume = 1.0f;
static bool muted = 0;

/* HUD */
static const Color TEXT_COLOR[3] = { RED, WHITE, GREEN }; /* low, normal, high, TODO: create an enum for this in main */

/* keyboard controls */
static const KeyboardKey KEY_MOVE_LEFT = KEY_A;
static const KeyboardKey KEY_MOVE_RIGHT = KEY_D;
static const KeyboardKey KEY_JUMP = KEY_SPACE;
static const KeyboardKey KEY_SHOOT_ALT = KEY_R;
static const KeyboardKey KEY_USE_PICKUP = KEY_Q;
static const KeyboardKey KEY_MUTE = KEY_F1;
static const KeyboardKey KEY_VOL_UP = KEY_F3;
static const KeyboardKey KEY_VOL_DOWN = KEY_F2;

//mouse controls
static const MouseButton BUTTON_SHOOT = MOUSE_LEFT_BUTTON;

//gameplay
#define NUM_PLATFORMS 10
#define NUM_HEALTH_PACKS 2

#endif 
