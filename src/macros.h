#ifndef MACROS_H
#define MACROS_H

#define SCREEN_MIDDLE(X) (int)(SCREEN_HEIGHT/2)-(int)(X.tx->height/2)
#define MIDDLE_X(X) ((int)(X.tx->width/2))
#define MIDDLE_Y(X) ((int)(X.tx->height/2))
#define IS_VISIBLE(X) ((X.x+X.tx->width>0 && X.x<SCREEN_WIDTH) && (X.y+X.tx->height>0 && X.y<SCREEN_WIDTH))
#define COLLISION(X,Y) ((X.x+X.tx->width>Y.x && X.x<Y.x+Y.tx->width) && (X.y+X.tx->height>Y.y && X.y<Y.y+Y.tx->height))
#define MOUSE_HOVER_BUTTON(X,Y) ((Y.x>X.x && Y.x<X.x+X.tx->width) && (Y.y>X.y && Y.y<X.y+X.tx->height))
#define DRAW(X) DrawTexture(*X.tx,X.x,X.y,WHITE)  
//X, flip horizontal, flip vertical, rotation
#define DRAW_PRO(X,FH,FV,R) DrawTexturePro(*X.tx,(Rectangle){.x=0,.y=0,.width=X.tx->width*FH,.height=X.tx->height*FV},(Rectangle){.x=X.x,.y=X.y,.width=X.tx->width,.height=X.tx->height},(Vector2){.x=0,.y=0},R,WHITE);

#endif
