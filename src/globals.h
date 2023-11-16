#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdint.h>

typedef int8_t i8;
typedef uint8_t ui8;
typedef unsigned short us;

static const char *DIRECTORY="res/";
static const unsigned short SCREENWIDTH=1920;
static const unsigned short SCREENHEIGHT=1080;
static const char *VERSION="1.3.2";

#define SCREENMIDDLE(X) (int)(SCREENHEIGHT/2)-(int)(X.tx.height/2)

#endif
