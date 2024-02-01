#ifndef GLOBALS_H
#define GLOBALS_H

static const int MAX_ROCKETS=3;
static const int MAX_PARTICLES=MAX_ROCKETS*2;
#define NUM_BG 11
#define NUM_PICKUP 2
#define NUM_MUSIC 3

enum {STANDING, WALKING, JUMPING};
enum {MENU, IN_PROGRESS, OVER};
enum {NONE, PARACHUTE, CRIT};
enum {NORMAL, HOVER};

static const char *DIRECTORY="/usr/local/share/rocketman/";
static const char *VERSION="3.0.0";

#endif
