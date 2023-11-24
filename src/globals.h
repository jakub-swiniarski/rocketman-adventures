#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdint.h>

typedef int8_t i8;
typedef uint8_t ui8;
typedef unsigned short us;

static const char *VERSION="1.4.6";

#define PATH(X) "res/" X
#define SCREENMIDDLE(X) (int)(SCREENHEIGHT/2)-(int)(X.tx.height/2)
#define MIDDLEX(X) ((int)(X.tx.width/2))
#define MIDDLEY(X) ((int)(X.tx.height/2))
#define VISIBLE(X) ((X.x+X.tx.width>0 && X.x<SCREENWIDTH) && (X.y+X.tx.height>0 && X.y<SCREENWIDTH))
#define COLLISION(X,Y) ((X.x+X.tx.width>Y.x && X.x<Y.x+Y.tx.width) && (X.y+X.tx.height>Y.y && X.y<Y.y+Y.tx.height))
#define MOUSEHOVERBUTTON(X,Y) ((Y.x>X.x && Y.x<X.x+X.tx[0].width) && (Y.y>X.y && Y.y<X.y+X.tx[0].height))

#endif
