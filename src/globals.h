#ifndef GLOBALS_H
#define GLOBALS_H

static const int MAX_ROCKETS=3;
static const int MAX_PARTICLES=MAX_ROCKETS*2;
#define NUM_BG 9
#define NUM_PICKUP 2

enum {Standing, Walking, Jumping};
enum {Menu, Normal, Over};

static const char *DIRECTORY="res/";
static const char *VERSION="2.0.0";

#endif
