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

bool rocketCollisionCheck(Rocket *r){
    if(r->x<=0 ||
    r->x+r->tx.width>=screenWidth ||
    r->y<=0 ||
    r->y+r->tx.height>=screenHeight){
        return true;
    }
    else{
        return false;
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

void platformCollisionCheck(Platform *p, Soldier *s){
    if(s->x+s->tx.width>p->x && s->x<p->x+p->tx.width){
        if(s->y+s->tx.height<p->y+p->tx.height 
        && s->y+s->tx.height>=p->y){
            s->y=p->y-s->tx.height;
            s->speedY=0;
            s->falling=0;
        }
    }
}
