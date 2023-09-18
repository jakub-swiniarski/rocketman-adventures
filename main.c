#include "raylib.h"
#include <stdlib.h>
#include <string.h>

//global variables
const char *directory="assets/";
char *path;

//function declarations
char*pathToFile(char *str);

//structs
typedef struct{
    Image img;
    Texture tx;
    int x,y;
} Soldier;

//function implementations
char *pathToFile(char *str){
    path=malloc(sizeof(char)*strlen(directory)+strlen("soldier.png")+1);
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
  
    Soldier redSoldier = {
        .img=LoadImage(pathToFile("soldier.png")), 
        .x=100,
        .y=100
    };
    free(path);
    ImageResizeNN(&redSoldier.img,12*5,20*5); 
    redSoldier.tx=LoadTextureFromImage(redSoldier.img);
        
    while(!WindowShouldClose()){
        BeginDrawing();

        DrawTexture(redSoldier.tx,redSoldier.x,redSoldier.y,WHITE);

        EndDrawing();
    }

    UnloadTexture(redSoldier.tx);

    CloseWindow();

    return 0;
}
