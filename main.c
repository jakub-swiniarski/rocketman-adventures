#include "raylib.h"
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <math.h>
#include <time.h>

//global variables
const char *directory="assets/";
char *path;

const int screenWidth=1280;
const int screenHeight=720;

//structs
static struct{
    Image redSoldier;
    Image rocket;
    Image particleSmoke;
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
    unsigned short rotation;
    short speedX,speedY;
} Rocket;

typedef struct{
    Texture tx;
    float x,y;
    unsigned short rotation;
    u_int8_t alpha;
    float cooldownAlpha;
} Particle;

//function declarations
char *pathToFile(char *str);
bool rocketBorderCheck(Rocket *r);
void soldierBorderCheck(Soldier *s);

//function implementations
char *pathToFile(char *str){
    path=malloc(sizeof(char)*strlen(directory)+strlen(str)+1);
    strcpy(path,directory);
    strcat(path,str);
    
    return path;
}

bool rocketBorderCheck(Rocket *r){
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

void soldierBorderCheck(Soldier *s){
    //horizontal
    if(s->x<0){
        s->x=0;
    }
    else if(s->x+s->tx.width>screenWidth){
        s->x=screenWidth-s->tx.width;
    }

    //vertical
    if(s->y<0){
        s->y=0;
    }
    else if(s->y+s->tx.height>screenHeight){
        s->y=screenHeight-s->tx.height;
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
    ImageResizeNN(&Images.rocket,30*3,8*3);

    Images.particleSmoke=LoadImage(pathToFile("particle_smoke.png"));
    ImageResizeNN(&Images.particleSmoke,12*10,12*10);

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

    UnloadImage(Images.redSoldier);
     
    u_int8_t numRockets=0;
    Rocket* rockets=malloc(numRockets*sizeof(Rocket));

    int numParticles=0;
    Particle* particles=malloc(numParticles*sizeof(Particle));

    //game loop
    while(!WindowShouldClose()){
        dt=GetFrameTime();

        for(int i=0; i<numRockets; i++){
            if(rocketBorderCheck(&rockets[i])){
                //smoke particles
                //for(int j=0; j<3; j++){
                    numParticles++;

                    Particle* newParticle=malloc(sizeof(Particle));

                    newParticle->tx=LoadTextureFromImage(Images.particleSmoke);

                    //srand(time(NULL));
                    newParticle->x=rockets[i].x;//+rand()%(50-(-50)+1)-50;
                    newParticle->y=rockets[i].y;//+rand()%(50-(-50)+1)-50;

                    newParticle->rotation=rand()%361;

                    newParticle->alpha=255;
                    newParticle->cooldownAlpha=0;

                    particles[numParticles-1]=*newParticle;

                    free(newParticle);
                //}

                //rocket jump
                redSoldier.speedX=-rockets[i].speedX;
                redSoldier.speedY=-rockets[i].speedY;

                //delete rockets
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

                break;
            }
        }

        //delete particles
        for(int i=0; i<numParticles; i++){
            if(particles[i].alpha<20){
                numParticles--;

                //shift elements in array
                for(int j=i; j<numParticles; j++){
                    particles[j]=particles[j+1];
                }

                Particle* buffer=malloc(sizeof(Particle)*numParticles);
                for(int j=0; j<numParticles; j++){
                    buffer[j]=particles[j];
                }
                particles=buffer;

                //TODO: rewrite array to buffer, ignoring the index that you want to get rid of, assign particles to buffer;

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

            //calculate rotataion
            newRocket->rotation=90-atan2((redSoldier.x+redSoldier.tx.width/2-GetMouseX()),(redSoldier.y+redSoldier.tx.height/2-GetMouseY()))*180/PI;
            /*if(GetMouseY()>redSoldier.y+redSoldier.tx.height){
                newRocket->rotataion=newRocket->rotataion+180;
            }*/
            newRocket->speedX=-cos(newRocket->rotation*PI/180)*800;
            newRocket->speedY=-sin(newRocket->rotation*PI/180)*800;
            /*if(newRocket->rotataion>=320){
                newRocket->speedX*=1;
                newRocket->speedY*=1;
            }*/
            
            rockets[numRockets-1]=*newRocket;
            
            free(newRocket);
        }
        if(IsKeyDown(KEY_D)){
            redSoldier.x+=150*dt;
        }
        if(IsKeyDown(KEY_A)){
            redSoldier.x-=150*dt;
        }
        if(IsKeyDown(KEY_SPACE) && redSoldier.y+redSoldier.tx.height>=screenHeight){
            redSoldier.speedY=-300;
        }

        if(redSoldier.speedX>0){
            redSoldier.speedX-=5;
        }
        else if(redSoldier.speedX<0){
            redSoldier.speedX+=5;
        }
       
        soldierBorderCheck(&redSoldier);

        //update player position
        redSoldier.x+=redSoldier.speedX*dt;
        redSoldier.y+=redSoldier.speedY*dt;

        //update cooldowns
        if(redSoldier.cooldown>0.f){
            redSoldier.cooldown-=GetFrameTime();
        }

        //update rockets
        for(int i=0; i<numRockets; i++){
            //position
            rockets[i].x+=rockets[i].speedX*dt;
            rockets[i].y+=rockets[i].speedY*dt;
        }  

        ClearBackground(BLACK); 
        BeginDrawing();

        DrawTexture(redSoldier.tx,redSoldier.x,redSoldier.y,WHITE);

        //draw particles
        for(int i=0; i<numParticles; i++){
            particles[i].cooldownAlpha-=GetFrameTime();

            if(particles[i].cooldownAlpha<=0){
                particles->cooldownAlpha=0.01;
                particles->alpha-=2;
            }

            DrawTexturePro(
                particles[i].tx,
                (Rectangle){ //src
                    .x=0,
                    .y=0,
                    .width=particles[i].tx.width,
                    .height=particles[i].tx.height
                },
                (Rectangle){ //dest
                    .x=particles[i].x,
                    .y=particles[i].y,
                    .width=particles[i].tx.width,
                    .height=particles[i].tx.height
                },
                (Vector2){ //origin
                    .x=particles[i].tx.width/2,
                    .y=particles[i].tx.height/2
                },
                particles[i].rotation, //rotataion
                (Color){
                    255,
                    255,
                    255,
                    particles[i].alpha
                }
            );
        }

        //draw rockets
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
                rockets[i].rotation,
                WHITE
            );
        }

        EndDrawing();
    }
   
    //unload images
    UnloadImage(Images.rocket);
    UnloadImage(Images.particleSmoke);

    //unload textures
    UnloadTexture(redSoldier.tx); 
    for(int i=0; i<numRockets; i++){
        UnloadTexture(rockets[i].tx); 
    } 
    for(int i=0; i<numParticles; i++){
        UnloadTexture(particles[i].tx);
    }

    CloseWindow();

    return 0;
}
