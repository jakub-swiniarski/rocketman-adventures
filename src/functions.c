#include <raylib.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "functions.h"
#include "globals.h"
#include "config.h"

void rocketBorderCheck(Rocket *r){
    if(r->y+r->tx.height>=SCREENHEIGHT)
        r->collided=1;
    else if(
    r->x<=0 ||
    r->x>=SCREENWIDTH ||
    r->y<=0){
        r->collided=1;
        r->shouldExplode=0;
    }
}

void soldierBorderCheck(Soldier *s){
    //horizontal
    if(s->x<0)
        s->x=0;
    else if(s->x+s->tx.width>SCREENWIDTH)
        s->x=SCREENWIDTH-s->tx.width;
}

void platformCollisionCheckS(Platform *p, Soldier *s){
    if(s->x+s->tx.width>p->x && s->x<p->x+p->tx.width){
        if(s->y+s->tx.height>p->y && s->y+s->tx.height<p->y+p->tx.height+10){ //add a constant number to prevent the player from falling through
            s->y=p->y-s->tx.height;
            s->speedY=0;
            s->falling=0;
        }
    }
}

void platformCollisionCheckR(Platform *p, Rocket *r){
    if(r->x+r->tx.width>p->x && r->x<p->x+p->tx.width){
        if(r->y+r->tx.height>p->y 
        && r->y<p->y+p->tx.height)
            r->collided=1;
    } 
}

bool pickupCollectCheck(Pickup *p, Soldier *r){
    if(r->x+r->tx.width>p->x && r->x<p->x+p->tx.width){
        if(r->y+r->tx.height>p->y && r->y<p->y+p->tx.height){
            if(r->pickup==0){
                r->pickup=p->id;
                p->x=-100;
                p->y=-100;
                return 1;
            }
        }
    }
    return 0;
}

void drawTextFull(const char *text, short x, short y, us fontSize, Color color){
    DrawText(text,x,y,fontSize,BLACK);
    DrawText(text,x+7,y+7,fontSize,color);
}

void drawTextFullCenter(const char *text, short y, us fontSize, Color color){
    drawTextFull(
        text,
        (int)(SCREENWIDTH/2)-(int)(MeasureTextEx(GetFontDefault(),text,fontSize,10).x/2),
        y,
        fontSize,
        color
    );
}
