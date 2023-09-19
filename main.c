#include "raylib.h"
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

//global variables
const char *directory="assets/";
char *path;

//function declarations
char *pathToFile(char *str);

//structs
typedef struct{
    Image img;
    Texture tx;
    float x,y;
    float speedX, speedY; //used for gravity and jumping
} Soldier;

typedef struct{
    Image img;
    Texture tx;
    float x,y;
    unsigned short id;
} Rocket;

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
     
    int numRockets=0;
    Rocket* rockets=malloc(numRockets*sizeof(Rocket));

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
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            numRockets++;
            rockets=realloc(rockets,numRockets*sizeof(Rocket));
            
            Rocket* newRocket=malloc(sizeof(Rocket));
            newRocket->img=LoadImage(pathToFile("rocket.png"));
            ImageResizeNN(&newRocket->img,20*2,6*2);
            newRocket->tx=LoadTextureFromImage(newRocket->img);
            newRocket->x=100*numRockets;
            newRocket->y=100;
            newRocket->id=numRockets-1;
            rockets[numRockets-1]=*newRocket;

            //REMOVE ROCKETS: ADD ID VARIABLE TO STRUCT, FREE THAT ARRAY INDEX
        }
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
        for(int i=0; i<numRockets; i++){
            DrawTexture(rockets[i].tx,rockets[i].x,rockets[i].y,WHITE);
        }

        EndDrawing();
    }
    
    //unload images
    UnloadImage(redSoldier.img); 

    //unload textuers
    UnloadTexture(redSoldier.tx);

    CloseWindow();

    return 0;
}
