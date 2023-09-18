#include "raylib.h"
#include <stdlib.h>
#include <string.h>

typedef struct{
    Texture img;
    int x,y;
} Soldier;

int main(void){
    //const char *directory="assets/";
    //char *path;

    const int screenWidth=1280;
    const int screenHeight=720;

    InitWindow(screenWidth,screenHeight,"Rocketman Adventures");
    
    int display=GetCurrentMonitor();
    SetWindowSize(GetMonitorWidth(display),GetMonitorHeight(display));
    ToggleFullscreen();
    
    SetTargetFPS(60);
   
    //strcat(path,directory);
    //strcat(path,"soldier.png");
    Soldier redSoldier = {
        .img=LoadTexture("assets/soldier.png"),
        .x=100,
        .y=100
    };
    //free(path);

    while(!WindowShouldClose()){
        BeginDrawing();

        DrawTexture(redSoldier.img,redSoldier.x,redSoldier.y,WHITE);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
