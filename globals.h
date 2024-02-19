#ifndef GLOBALS_H
#define GLOBALS_H

#define NUM_BG 11
#define NUM_MUSIC 3

enum { STANDING, WALKING, JUMPING }; /* player states, used for animations */
enum { MENU, IN_PROGRESS, OVER }; /* game states */
enum { NONE, PARACHUTE, CRIT, NUM_PICKUP }; /* pickups */
enum { NORMAL, HOVER }; /* button states */
enum { SFX_EXPLOSION, SFX_PICKUP, SFX_JUMP, SFX_DEATH, NUM_SFX }; /* sound effects */

static const char *DIRECTORY = "res/";
static const char *VERSION = "3.0.1";

#endif
