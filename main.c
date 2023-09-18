#include "raylib.h"
#include <stdlib.h>
#include <string.h>

//global variables
const char *directory="assets/";
char *path;

//function declarations
char*pathToFile(char *str);

//function implementations
char *pathToFile(char *str){
    path=malloc(sizeof(char)*strlen(directory)+strlen("soldier.png")+1);
    strcpy(path,directory);
    strcat(path,str);
    
    return path;
}

//structs
typedef struct{
    Texture img;
    int x,y;
} Soldier;

int main(void){
    const int screenWidth=1280;
    const int screenHeight=720;

    InitWindow(screenWidth,screenHeight,"Rocketman Adventures");
    
    int display=GetCurrentMonitor();
    SetWindowSize(GetMonitorWidth(display),GetMonitorHeight(display));
    ToggleFullscreen();
    
    SetTargetFPS(60);
  
    Soldier redSoldier = {
        .img=LoadTexture(pathToFile("soldier.png")),
        .x=100,
        .y=100
    };
    free(path);

    while(!WindowShouldClose()){
        BeginDrawing();

        DrawTexture(redSoldier.img,redSoldier.x,redSoldier.y,WHITE);

        EndDrawing();
    }

    UnloadTexture(redSoldier.img);

    CloseWindow();

    return 0;
}
