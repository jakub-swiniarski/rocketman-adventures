#include "raylib.h"

int main(void){
    const int screenWidth=1280;
    const int screenHeight=720;

    InitWindow(screenWidth,screenHeight,"Rocketman Adventures");
    
    int display=GetCurrentMonitor();
    SetWindowSize(GetMonitorWidth(display),GetMonitorHeight(display));
    ToggleFullscreen();
    
    SetTargetFPS(60);

    while(!WindowShouldClose()){
        BeginDrawing();

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
