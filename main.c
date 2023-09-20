#include "raylib.h"
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <math.h>

//global variables
const char *directory="assets/";
char *path;

const int screenWidth=1280;
const int screenHeight=720;

//structs
static struct{
    Image redSoldier;
    Image rocket;
} Images;

typedef struct{
    Texture tx;
    float x,y;
    float speedX, speedY; //used for gravity and jumping
    float cooldown;
} Soldier;

typedef struct{
    Texture tx;
    float x,y;
    unsigned short angle;
    short speedX,speedY;
    float particleCooldown;
} Rocket;

typedef struct{
    Texture tx;
    float x,y;
    //rotation?
    u_int8_t alpha; //TODO: delete particle if alpha==0
} Particle;

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

bool rocketBorderCheck(Rocket *r){ //TODO: extend borders so that rockets disappear before they get deleted
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

int main(void){
    InitWindow(screenWidth,screenHeight,"Rocketman Adventures");
    
    int display=GetCurrentMonitor();
    SetWindowSize(GetMonitorWidth(display),GetMonitorHeight(display));
    ToggleFullscreen();
    
    SetTargetFPS(60);
    float dt=1.f;
    
    //load and resize images
    Images.redSoldier=LoadImage(pathToFile("soldier.png"));
    ImageResizeNN(&Images.redSoldier,12*5,20*5);

    Images.rocket=LoadImage(pathToFile("rocket.png"));
    ImageResizeNN(&Images.rocket,20*3,6*3);

    free(path);

    //player
    Soldier redSoldier = {
        .x=100,
        .y=100,
        .speedX=0,
        .speedY=0,
        .cooldown=0.f
    };
    redSoldier.tx=LoadTextureFromImage(Images.redSoldier);
     
    u_int8_t numRockets=0;
    Rocket* rockets=malloc(numRockets*sizeof(Rocket));

    u_int8_t numParticles=0;
    Particle* particles=malloc(numParticles*sizeof(Particle));

    //game loop
    while(!WindowShouldClose()){
        ClearBackground(BLACK);

        dt=GetFrameTime();

        //delete rockets
        for(int i=0; i<numRockets; i++){
            if(rocketBorderCheck(&rockets[i])){
                numRockets--; 

                //shift elements in array
                for(int j=i; j<numRockets; j++){
                    rockets[j]=rockets[j+1];
                }
               
                //TEMPORARY SOLUTION 
                Rocket* buffer=malloc(sizeof(Rocket)*numRockets);
                for(int j=0; j<numRockets; j++){
                    buffer[j]=rockets[j];
                }
                rockets=buffer;

                //rockets=realloc(rockets,numRockets);

                break;
            }
        }

        //gravity
        if(redSoldier.y+redSoldier.tx.height>=screenHeight){
            redSoldier.y=screenHeight-redSoldier.tx.height;
            redSoldier.speedY=0;
        }
        else{
            redSoldier.speedY+=15;
        }

        //input
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && redSoldier.cooldown<=0){
            redSoldier.cooldown=0.5;
            numRockets++;
            rockets=realloc(rockets,numRockets*sizeof(Rocket));
            
            Rocket* newRocket=malloc(sizeof(Rocket));
            
            newRocket->tx=LoadTextureFromImage(Images.rocket);
            newRocket->x=redSoldier.x+redSoldier.tx.width/2;
            newRocket->y=redSoldier.y+redSoldier.tx.height/2;
            newRocket->particleCooldown=0.f;

            //calculate angle
            newRocket->angle=90-atan2((redSoldier.x+redSoldier.tx.width/2-GetMouseX()),(redSoldier.y+redSoldier.tx.height/2-GetMouseY()))*180/PI;
            /*if(GetMouseY()>redSoldier.y+redSoldier.tx.height){
                newRocket->angle=newRocket->angle+180;
            }*/
            newRocket->speedX=-cos(newRocket->angle*PI/180)*800;
            newRocket->speedY=-sin(newRocket->angle*PI/180)*800;
            /*if(newRocket->angle>=320){
                newRocket->speedX*=1;
                newRocket->speedY*=1;
            }*/
            
            rockets[numRockets-1]=*newRocket;
            
            free(newRocket);

            //TODO: REMOVE ROCKETS: ADD ID VARIABLE TO STRUCT, FREE THAT ARRAY INDEX
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

        //update cooldown
        if(redSoldier.cooldown>0.f){
            redSoldier.cooldown-=GetFrameTime();
        }

        //update rocket position
        for(int i=0; i<numRockets; i++){
            rockets[i].x+=rockets[i].speedX*dt;
            rockets[i].y+=rockets[i].speedY*dt;
        }  

        BeginDrawing();

        DrawTexture(redSoldier.tx,redSoldier.x,redSoldier.y,WHITE);
        for(int i=0; i<numRockets; i++){
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
    UnloadImage(Images.redSoldier);
    UnloadImage(Images.rocket);

    //unload textures
    UnloadTexture(redSoldier.tx); 
    for(int i=0; i<numRockets; i++){
        UnloadTexture(rockets[i].tx); 
    } 

    CloseWindow();

    return 0;
}
