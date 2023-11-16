#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdint.h>

typedef int8_t i8;
typedef uint8_t ui8;
typedef unsigned short us;

static const char *DIRECTORY="/usr/share/rocketman/";
static const unsigned short SCREENWIDTH=1920;
static const unsigned short SCREENHEIGHT=1080;
static const char *VERSION="1.3.3";

#define SCREENMIDDLE(X) (int)(SCREENHEIGHT/2)-(int)(X.tx.height/2)
#define MIDDLEX(X) ((int)(X.tx.width/2))
#define MIDDLEY(X) ((int)(X.tx.height/2))

#endif
