#include "raylib.h"
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <stdio.h>

//global variables
const char *directory="assets/";
char *path;

//function declarations
char*pathToFile(char *str);

//structs
typedef struct{
    Image img;
    Texture tx;
    float x,y;
    float speedX, speedY; //used for gravity and jumping
} Soldier;

//function implementations
char *pathToFile(char *str){
    path=malloc(sizeof(char)*strlen(directory)+strlen(str)+1);
    strcpy(path,directory);
    strcat(path,str);
    
    return path;
}

int main(void){
    const int screenWidth=1280;
    const int screenHeight=720;

    InitWindow(screenWidth,screenHeight,"Rocketman Adventures");
    
    int display=GetCurrentMonitor();
    SetWindowSize(GetMonitorWidth(display),GetMonitorHeight(display));
    ToggleFullscreen();
    
    SetTargetFPS(60);
    float dt=1.f;
 
    //player
    Soldier redSoldier = {
        .img=LoadImage(pathToFile("soldier.png")), 
        .x=100,
        .y=100,
        .speedX=0,
        .speedY=0
    };
    free(path);
    ImageResizeNN(&redSoldier.img,12*5,20*5); 
    redSoldier.tx=LoadTextureFromImage(redSoldier.img);
      
    //game loop
    while(!WindowShouldClose()){
        ClearBackground(BLACK);

        dt=GetFrameTime();

        //gravity
        if(redSoldier.y+redSoldier.tx.height>=screenHeight){
            redSoldier.y=screenHeight-redSoldier.tx.height;
            redSoldier.speedY=0;
        }
        else{
            redSoldier.speedY+=15;
        }
 

        //input
        if(IsKeyDown(KEY_D) && redSoldier.speedX==0){
            redSoldier.x+=150*dt;
        }
        if(IsKeyDown(KEY_A) && redSoldier.speedX==0){
            redSoldier.x-=150*dt;
        }
        if(IsKeyDown(KEY_SPACE) && redSoldier.y+redSoldier.tx.height>=screenHeight){
            redSoldier.speedY=-300;
        }
        
        //update player position
        redSoldier.x+=redSoldier.speedX*dt;
        redSoldier.y+=redSoldier.speedY*dt;

        BeginDrawing();

        DrawTexture(redSoldier.tx,redSoldier.x,redSoldier.y,WHITE);

        EndDrawing();
    }
    
    //unload images
    UnloadImage(redSoldier.img); 

    //unload textuers
    UnloadTexture(redSoldier.tx);

    CloseWindow();

    return 0;
}
