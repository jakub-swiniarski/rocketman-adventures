#include "raylib.h"
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

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
    float speedX, speedY; //used for gravity and rocket jumping
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

        //input
        if(IsKeyDown(KEY_D) && redSoldier.speedX==0){
            redSoldier.x+=5;
        }
        if(IsKeyDown(KEY_A) && redSoldier.speedX==0){
            redSoldier.x-=5;
        }
        
        //gravity
        if(redSoldier.y+redSoldier.tx.height>=screenHeight){
            redSoldier.y=screenHeight-redSoldier.tx.height;
            redSoldier.speedY=0;
        }
        else{
            redSoldier.speedY+=0.5;
        }
        
        //update player position
        redSoldier.x+=redSoldier.speedX;
        redSoldier.y+=redSoldier.speedY;

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
