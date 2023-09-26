#include <stdlib.h>
#include <string.h>

#include "../headers/functions.h"
#include "../headers/globals.h"

//function implementations
char *pathToFile(char *str){
    char *path=malloc(sizeof(char)*strlen(directory)+strlen(str)+1);
    strcpy(path,directory);
    strcat(path,str);
    
    return path;
}

void rocketBorderCheck(Rocket *r){
    if(r->x<=0 ||
    r->x+r->tx.width>=screenWidth ||
    r->y<=0 ||
    r->y+r->tx.height>=screenHeight){
        r->collided=1;
    }
}

void soldierBorderCheck(Soldier *s){
    //horizontal
    if(s->x<0){
        s->x=0;
    }
    else if(s->x+s->tx.width>screenWidth){
        s->x=screenWidth-s->tx.width;
    }

    //vertical
    /*if(s->y<0){
        s->y=0;
    }*/
    /*else if(s->y+s->tx.height>screenHeight){
        s->y=screenHeight-s->tx.height;
    }*/
}

void platformCollisionCheckS(Platform *p, Soldier *s){
    if(s->x+s->tx.width>p->x && s->x<p->x+p->tx.width){
        if(s->y+s->tx.height<p->y+p->tx.height 
        && s->y+s->tx.height>=p->y){
            s->y=p->y-s->tx.height;
            s->speedY=0;
            s->falling=0;
        }
    }
}

void platformCollisionCheckR(Platform *p, Rocket *r){
    if(r->x+r->tx.width>p->x && r->x<p->x+p->tx.width){
        r->collided=1;
    } 
}
