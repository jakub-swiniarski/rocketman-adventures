#ifndef MACROS_H
#define MACROS_H

#define SCREEN_MIDDLE(X) (int)(SCREEN_HEIGHT/2)-(int)(X.tx->height/2)
#define MIDDLE_X(X) ((int)(X.tx->width/2))
#define MIDDLE_Y(X) ((int)(X.tx->height/2))
#define IS_VISIBLE(X) ((X.x+X.tx->width>0 && X.x<SCREEN_WIDTH) && (X.y+X.tx->height>0 && X.y<SCREEN_WIDTH))
#define COLLISION(X,Y) ((X.x+X.tx->width>Y.x && X.x<Y.x+Y.tx->width) && (X.y+X.tx->height>Y.y && X.y<Y.y+Y.tx->height))
#define MOUSE_HOVER_BUTTON(X,Y) ((Y.x>X.x && Y.x<X.x+X.tx->width) && (Y.y>X.y && Y.y<X.y+X.tx->height))
#define DRAW(X) DrawTexture(*X.tx,X.x,X.y,WHITE)  

#endif
