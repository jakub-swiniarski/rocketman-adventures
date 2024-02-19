#ifndef GLOBALS_H
#define GLOBALS_H

#define NUM_BG 11
#define NUM_PICKUP 2
#define NUM_MUSIC 3
#define NUM_SFX 4

enum {STANDING, WALKING, JUMPING}; //player states, used for animations
enum {MENU, IN_PROGRESS, OVER}; //game states
enum {NONE, PARACHUTE, CRIT}; //pickups
enum {NORMAL, HOVER}; //button states
enum {SFX_EXPLOSION, SFX_PICKUP, SFX_JUMP, SFX_DEATH}; //sound effects

static const char *DIRECTORY="res/";
static const char *VERSION="3.0.1";

#endif
