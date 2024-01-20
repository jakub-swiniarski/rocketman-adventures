#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "functions.h"
#include "globals.h"
#include "config.h"

char *path_to_file(char *name){
    char *path=malloc(sizeof(char)*strlen(DIRECTORY)+strlen(name)+1);
    sprintf(path,"%s%s",DIRECTORY,name);
    
    return path;
}


void rocket_border_check(Rocket *r){
    if(r->y+r->tx->height>=SCREEN_HEIGHT)
        r->collided=1;
    else if(
    r->x<=0 ||
    r->x>=SCREEN_WIDTH ||
    r->y<=0){
        r->collided=1;
        r->should_explode=0;
    }
}

void soldier_border_check(Soldier *s){
    //horizontal
    if(s->x<0)
        s->x=0;
    else if(s->x+s->tx->width>SCREEN_WIDTH)
        s->x=SCREEN_WIDTH-s->tx->width;
}

void platform_collision_check_soldier(Platform *p, Soldier *s){
    if(s->x+s->tx->width>p->x && s->x<p->x+p->tx->width){
        if(s->y+s->tx->height>p->y && s->y+s->tx->height<p->y+p->tx->height+10){ //add a constant number to prevent the player from falling through
            s->y=p->y-s->tx->height;
            s->speed_y=0;
            s->falling=0;
        }
    }
}

void platform_collision_check_rocket(Platform *p, Rocket *r){
    if(r->x+r->tx->width>p->x && r->x<p->x+p->tx->width){
        if(r->y+r->tx->height>p->y 
        && r->y<p->y+p->tx->height)
            r->collided=1;
    } 
}

bool pickup_collect_check(Pickup *p, Soldier *r){
    if(r->x+r->tx->width>p->x && r->x<p->x+p->tx->width){
        if(r->y+r->tx->height>p->y && r->y<p->y+p->tx->height){
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

void draw_text_full(const char *text, int x, int y, int font_size, Color color){
    DrawText(text,x,y,font_size,BLACK);
    DrawText(text,x+7,y+7,font_size,color);
}

void draw_text_full_center(const char *text, int y, int font_size, Color color){
    draw_text_full(
        text,
        (int)(SCREEN_WIDTH/2)-(int)(MeasureTextEx(GetFontDefault(),text,font_size,10).x/2),
        y,
        font_size,
        color
    );
}
