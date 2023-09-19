#include "raylib.h"
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <math.h>

//global variables
const char *directory="assets/";
char *path;

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
    unsigned short angle;
    short speedX,speedY;
} Rocket;

//function declarations
char *pathToFile(char *str);
bool rocketBorderCheck(Rocket *r);

//function implementations
char *pathToFile(char *str){
    path=malloc(sizeof(char)*strlen(directory)+strlen(str)+1);
    strcpy(path,directory);
    strcat(path,str);
    
    return path;
}

/*bool rocketBorderCheck(Rocket *r){

}*/

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
            ImageResizeNN(&newRocket->img,20*3,6*3);
            newRocket->tx=LoadTextureFromImage(newRocket->img);
            newRocket->x=redSoldier.x+redSoldier.tx.width/2-newRocket->x/2;
            newRocket->y=redSoldier.y+redSoldier.tx.height/2-newRocket->y/2;
            newRocket->id=numRockets-1;

            //calculate angle
            newRocket->angle=90-atan((redSoldier.x+redSoldier.tx.width/2-GetMouseX())/(redSoldier.y+redSoldier.tx.height/2-GetMouseY()))*180/PI;
            if(GetMouseY()>redSoldier.y+redSoldier.tx.height){
                newRocket->angle*=360-newRocket->angle;
            }
            //newRocket->speedX=-100*(redSoldier.x+redSoldier.tx.width/2-GetMouseX())/newRocket->angle;
            //newRocket->speedY=-100*(redSoldier.y+redSoldier.tx.height/2-GetMouseY())/newRocket->angle;
            newRocket->speedX=0;
            newRocket->speedY=0;

            rockets[numRockets-1]=*newRocket;
            
            //free(newRocket);

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

        //update rocket position
        for(int i=0; i<numRockets; i++){
            rockets[i].x+=rockets[i].speedX*dt;
            rockets[i].y+=rockets[i].speedY*dt;
        }  

        BeginDrawing();

        DrawTexture(redSoldier.tx,redSoldier.x,redSoldier.y,WHITE);
        for(int i=0; i<numRockets; i++){
            //DrawTexture(rockets[i].tx,rockets[i].x,rockets[i].y,WHITE);
            DrawTexturePro(
                rockets[i].tx,
                (Rectangle){ //src
                    .x=0,
                    .y=0,
                    .width=rockets[i].tx.width,
                    .height=rockets[i].tx.height
                },
                (Rectangle){ //dest
                    .x=rockets[i].x,
                    .y=rockets[i].y,
                    .width=rockets[i].tx.width,
                    .height=rockets[i].tx.height
                },
                (Vector2){ //origin
                    .x=rockets[i].tx.width/2,
                    .y=rockets[i].tx.height/2
                },
                rockets[i].angle,
                WHITE
            );
        }

        EndDrawing();
    }
    
    //unload images
    UnloadImage(redSoldier.img); 
    //TODO: UNLOAD ROCKETS

    //unload textuers
    UnloadTexture(redSoldier.tx);

    CloseWindow();

    return 0;
}
