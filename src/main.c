#include <raylib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "../headers/structs.h"
#include "../headers/functions.h"
#include "../headers/globals.h"

int main(void){
    InitWindow(SCREENWIDTH,SCREENHEIGHT,"Rocketman Adventures");
    
    uint8_t display=GetCurrentMonitor();
    SetWindowSize(GetMonitorWidth(display),GetMonitorHeight(display));
    ToggleFullscreen();

    InitAudioDevice();

    SetTargetFPS(60);
    float dt=1.f;

    uint8_t gameState=0; //0 - not started, 1 - in progress, 2 - game over

    //load and resize images
    Images.redSoldier=LoadImage(pathToFile("red_soldier.png"));
    ImageResizeNN(&Images.redSoldier,12*5,20*5);

    Images.rocket=LoadImage(pathToFile("rocket.png"));
    ImageResizeNN(&Images.rocket,30*3,8*3);

    Images.launcher=LoadImage(pathToFile("launcher.png"));
    ImageResizeNN(&Images.launcher,20*5,8*5);

    Images.particleSmoke=LoadImage(pathToFile("particle_smoke.png"));
    ImageResizeNN(&Images.particleSmoke,12*10,12*10);

    Images.backgrounds[0]=LoadImage(pathToFile("background0.png"));
    ImageResizeNN(&Images.backgrounds[0],SCREENWIDTH,SCREENHEIGHT);

    Images.backgrounds[1]=LoadImage(pathToFile("background1.png"));
    ImageResizeNN(&Images.backgrounds[1],SCREENWIDTH,SCREENHEIGHT);

    Images.platform=LoadImage(pathToFile("platform.png"));
    ImageResizeNN(&Images.platform,30*5,2*5);

    Images.parachutePickup=LoadImage(pathToFile("parachute_pickup.png"));
    ImageResizeNN(&Images.parachutePickup, 16*4, 20*4);

    Images.parachute=LoadImage(pathToFile("parachute.png"));
    ImageResizeNN(&Images.parachute, 13*8, 11*8);

    Images.critPickup=LoadImage(pathToFile("crit_pickup.png"));
    ImageResizeNN(&Images.critPickup,9*8,13*8);

    //sfx
    Sound fxExplosion=LoadSound(pathToFile("explosion.ogg"));
    Sound fxPickup=LoadSound(pathToFile("pickup.ogg"));
    Music music=LoadMusicStream(pathToFile("soundtrack0.ogg"));

    //player
    Soldier redSoldier={
        .tx=LoadTextureFromImage(Images.redSoldier),
        .speedX=0,
        .speedY=0,
        .cooldown=0,
        .falling=0,
        .slowfall=1,
        .critBoost=1
    };
    redSoldier.x=(int)(SCREENWIDTH/2)-redSoldier.tx.width;
    redSoldier.y=SCREENHEIGHT-redSoldier.tx.height;
    UnloadImage(Images.redSoldier);  

    //parachute
    Texture parachute=LoadTextureFromImage(Images.parachute);
    UnloadImage(Images.parachute);
    int8_t rotationParachute=0; 

    //rocket launcher
    Launcher rl={
        .tx=LoadTextureFromImage(Images.launcher),
        .x=0,
        .y=0,
        .rotation=0,
        .color=WHITE
    };
    UnloadImage(Images.launcher);

    //background
    Texture backgrounds[2];
    short bgY[2]; 
    for(uint8_t i=0; i<2; i++){
        backgrounds[i]=LoadTextureFromImage(Images.backgrounds[i]);
        UnloadImage(Images.backgrounds[i]);
        bgY[i]=-i*SCREENHEIGHT;
    }

    srand(time(NULL));

    uint8_t numRockets=0;
    Rocket* rockets=malloc(numRockets*sizeof(Rocket));

    uint8_t numParticles=0;
    Particle* particles=malloc(numParticles*sizeof(Particle));

    uint8_t numPlatforms=5;
    Platform platforms[numPlatforms];
    for(uint8_t i=0; i<numPlatforms; i++){
        Platform newPlatform={
            .tx=LoadTextureFromImage(Images.platform),
            .x=rand()%(SCREENWIDTH/2+1)+SCREENWIDTH/4, //this is also used for random x when moving platform to the top
            .y=SCREENHEIGHT-(i+1)*200
        };

        platforms[i]=newPlatform;
    }
    UnloadImage(Images.platform);
    short bgShift=0;

    //pickups
    Pickup pickup={
        .tx=LoadTextureFromImage(Images.parachutePickup),
        .x=-100,
        .y=-100,
        .id=1
    };

    unsigned short score=0;
    char scoreString[5];

    //TODO: SHINE PARTICLES FOR PICKUPS

    PlayMusicStream(music);

    //game loop
    while(!WindowShouldClose()){
        dt=GetFrameTime();
        UpdateMusicStream(music);

        for(uint8_t i=0; i<numRockets; i++){
            rocketBorderCheck(&rockets[i]);

            if(rockets[i].collided){
                PlaySound(fxExplosion);
                UnloadTexture(rockets[i].tx);

                //smoke particles
                if(rockets[i].shouldExplode){
                    for(uint8_t j=0; j<3; j++){
                        numParticles++;
                        Particle *buffer=malloc(sizeof(Particle)*numParticles);
                    
                        Particle newParticle={
                            .tx=LoadTextureFromImage(Images.particleSmoke),
                            .x=rockets[i].x+(int)(rockets[i].tx.width/2)+rand()%(50-(-50)+1)-50,
                            .y=rockets[i].y+(int)(rockets[i].tx.width/2)+rand()%(50-(-50)+1)-50,
                            .rotation=rand()%361,
                            .alpha=255
                        };

                        for(uint8_t i=0; i<numParticles-1; i++){
                            buffer[i]=particles[i];
                        }

                        buffer[numParticles-1]=newParticle;
                        particles=buffer;
                    }

                    //rocket jump
                    if(abs((int)(redSoldier.x+(int)(redSoldier.tx.width/2)-rockets[i].x-(int)(rockets[i].tx.width/2)))<100 
                    && abs((int)(redSoldier.y+(int)(redSoldier.tx.height/2)-rockets[i].y-(int)(rockets[i].tx.height/2)))<100
                    && gameState!=2){
                        redSoldier.speedX=redSoldier.critBoost*-1*rockets[i].speedX;
                        redSoldier.speedY=redSoldier.critBoost*-1*rockets[i].speedY; 
                    }

                    if(redSoldier.pickupActive==2){
                        redSoldier.pickupActive=0;
                    }      
                }

                //delete rockets
                numRockets--; 

                //shift elements in array
                for(uint8_t j=i; j<numRockets; j++){
                    rockets[j]=rockets[j+1];
                }
               
                //TEMPORARY SOLUTION 
                Rocket* buffer=malloc(sizeof(Rocket)*numRockets);
                for(uint8_t j=0; j<numRockets; j++){
                    buffer[j]=rockets[j];
                }
                rockets=buffer;

                break;
            }
        }

        //delete particles
        for(uint8_t i=0; i<numParticles; i++){
            if(particles[i].alpha<5){
                numParticles--;

                //shift elements in array
                for(uint8_t j=i; j<numParticles; j++){
                    particles[j]=particles[j+1];
                }

                Particle* buffer=malloc(sizeof(Particle)*numParticles);
                for(uint8_t j=0; j<numParticles; j++){
                    buffer[j]=particles[j];
                }
                particles=buffer;

                break;
            }
        }

        //movement
        if(gameState!=2){
            if(IsKeyDown(KEY_D) && !IsKeyDown(KEY_A)){
                redSoldier.x+=150*dt;

                if(redSoldier.pickupActive==1 && rotationParachute>-30){
                    rotationParachute-=60*dt;
                }
            }
            if(IsKeyDown(KEY_A) && !IsKeyDown(KEY_D)){
                redSoldier.x-=150*dt;
            
                if(redSoldier.pickupActive==1 && rotationParachute<30){
                    rotationParachute+=60*dt;
                } 
            }
            //reset parachute rotation
            if(!IsKeyDown(KEY_A) && !IsKeyDown(KEY_D)){ //if not moving horizontally
                if(rotationParachute>0){
                    rotationParachute-=100*dt;
                } 
                else if(rotationParachute<0){
                    rotationParachute+=100*dt;
                }
            }
            if(IsKeyDown(KEY_SPACE) && !redSoldier.falling){
                if(redSoldier.pickupActive==1){
                    redSoldier.pickupActive=0;
                }
                redSoldier.falling=0;
                redSoldier.speedY=-300;
            }

            //update rocket launcher
            rl.x=redSoldier.x+(int)(redSoldier.tx.width/2);
            rl.y=redSoldier.y+(int)(redSoldier.tx.height/2); 
            rl.rotation=270-atan2((redSoldier.x+(int)(redSoldier.tx.width/2)-GetMouseX()),(redSoldier.y+(int)(redSoldier.tx.height/2)-GetMouseY()))*180/PI; 
        
            //flip the rocket launcher to prevent it from going upside down
            if(GetMouseX()<redSoldier.x+(int)(redSoldier.tx.width/2)){
                rl.flip=-1;
            }
            else{
                rl.flip=1;
            }
        }

        //update player position
        redSoldier.x+=redSoldier.speedX*dt;
        if(redSoldier.speedY>0){
            if(redSoldier.falling){
                redSoldier.y+=redSoldier.speedY*dt*redSoldier.slowfall; 
            }
        }
        else{
            redSoldier.y+=redSoldier.speedY*dt; 
        } 

        if(redSoldier.y<(int)(SCREENHEIGHT/2)-(int)(redSoldier.tx.height/2)){
            //score
            score-=redSoldier.speedY*dt;
            sprintf(scoreString, "%hu", score);

            redSoldier.y=(int)(SCREENHEIGHT/2)-(int)(redSoldier.tx.height/2); 
            
            if(gameState==0){
                gameState=1;
            }
        }

        //gravity
        if(redSoldier.y+redSoldier.tx.height>=SCREENHEIGHT){
            if(gameState!=1){
                redSoldier.y=SCREENHEIGHT-redSoldier.tx.height;
                redSoldier.speedY=0;
                redSoldier.falling=0;
            }

            if(gameState==1){
                gameState=2;
            }
        }
        else{
            redSoldier.falling=1;
            redSoldier.speedY+=15;
        }

        //input
        if((IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsKeyPressed(KEY_R)) && redSoldier.cooldown<0 && gameState!=2){
            redSoldier.cooldown=120;
            numRockets++;

            Rocket *buffer=malloc(sizeof(Rocket)*numRockets);

            Rocket newRocket={
                .tx=LoadTextureFromImage(Images.rocket),
                .x=redSoldier.x+(int)(redSoldier.tx.width/2),
                .y=redSoldier.y+(int)(redSoldier.tx.height/2),
                .rotation=90-atan2((redSoldier.x+(int)(redSoldier.tx.width/2)-GetMouseX()),(redSoldier.y+(int)(redSoldier.tx.height/2)-GetMouseY()))*180/PI,
                .collided=0,
                .shouldExplode=1
            };
            
            newRocket.speedX=-1.2*cos(newRocket.rotation*PI/180)*800;
            newRocket.speedY=-1.2*sin(newRocket.rotation*PI/180)*800;
            
            for(uint8_t i=0; i<numRockets-1; i++){
                buffer[i]=rockets[i];
            }

            buffer[numRockets-1]=newRocket;
            rockets=buffer;
        }
        //ACTIVATE PICKUP
        if(IsKeyPressed(KEY_Q)){
            redSoldier.pickupActive=redSoldier.pickup;
            redSoldier.pickup=0;
        }
       
        //horizontal friction
        if(redSoldier.speedX>0){
            if(redSoldier.speedX<5){
                redSoldier.speedX=0;
            }
            else{
                redSoldier.speedX-=8;
            }
        }
        else if(redSoldier.speedX<0){
            if(redSoldier.speedX>-5){
                redSoldier.speedX=0;
            }
            else{
                redSoldier.speedX+=8;
            }
        }

        soldierBorderCheck(&redSoldier);

        //update cooldowns
        redSoldier.cooldown-=150*GetFrameTime();

        //update rockets
        for(uint8_t i=0; i<numRockets; i++){
            //position
            rockets[i].x+=rockets[i].speedX*dt;
            rockets[i].y+=rockets[i].speedY*dt;
        }  

        //pickup effects
        switch(redSoldier.pickupActive){
            case 1:
                redSoldier.slowfall=0.2;
            break;

            case 2:
                redSoldier.critBoost=2;
                rl.color=RED;    
            break;

            default: //RESET
                redSoldier.slowfall=1;
                redSoldier.critBoost=1;
                rl.color=WHITE;
            break;
        }

        ClearBackground(BLACK); 
        BeginDrawing();

        //update background
        bgShift=redSoldier.speedY*dt/2;
        for(uint8_t i=0; i<2; i++){
            //parallax scrolling
            if(bgY[i]>SCREENHEIGHT){
                bgY[i]=-SCREENHEIGHT;
                bgY[1-i]=0;
            } 
            if(redSoldier.y==(int)(SCREENHEIGHT/2)-(int)(redSoldier.tx.height/2) && redSoldier.speedY<0){
                bgY[i]-=bgShift;
            }

            //draw background
            DrawTexture(backgrounds[i],0,bgY[i],WHITE);
        }

        //update platforms
        for(uint8_t i=0; i<numPlatforms; i++){
            //soldier collisions
            if(redSoldier.speedY>0){
                platformCollisionCheckS(&platforms[i],&redSoldier);
            }

            if(redSoldier.y==(int)(SCREENHEIGHT/2)-(int)(redSoldier.tx.height/2) && redSoldier.speedY<0){
                platforms[i].y-=redSoldier.speedY*dt;
            }

            //rocket collisions
            for(uint8_t j=0; j<numRockets; j++){
                platformCollisionCheckR(&platforms[i],&rockets[j]);
            }

            if(platforms[i].y>SCREENHEIGHT){
                platforms[i].x=rand()%(SCREENWIDTH/2+1)+SCREENWIDTH/4;
                platforms[i].y=-platforms[i].tx.height;
                if(!pickupVisible(&pickup)){
                    pickup.id=rand()%(2-1+1)+1;
                    switch(pickup.id){
                        case 1:
                            pickup.tx=LoadTextureFromImage(Images.parachutePickup);
                        break;
                        
                        case 2:
                            pickup.tx=LoadTextureFromImage(Images.critPickup);
                        break;
                    }

                    int pickupRand=rand()%(10-1+1)+1;
                    if(pickupRand==1){
                        pickup.y=platforms[i].y-pickup.tx.height;
                        pickup.x=platforms[i].x+platforms[i].tx.width/2-pickup.tx.width/2;
                    }
                }
            }

            //draw platforms
            DrawTexture(platforms[i].tx,platforms[i].x,platforms[i].y,WHITE);
        }

        //update pickup
        if(pickupCollectCheck(&pickup, &redSoldier)) PlaySound(fxPickup); 
        if(redSoldier.y==(int)(SCREENHEIGHT/2)-(int)(redSoldier.tx.height/2) && redSoldier.speedY<0){
            pickup.y-=redSoldier.speedY*dt; 
        } 
        if(pickupVisible(&pickup)){
            DrawTexture(pickup.tx,pickup.x,pickup.y,WHITE);
        }

        //parachute
        if(redSoldier.pickupActive==1){
            DrawTexturePro(
                parachute,
                (Rectangle){ //src
                    .x=0,
                    .y=0,
                    .width=parachute.width,
                    .height=parachute.height
                },
                (Rectangle){ //dest
                    .x=redSoldier.x+(int)(redSoldier.tx.width/2),
                    .y=redSoldier.y,
                    .width=parachute.width,
                    .height=parachute.height
                },
                (Vector2){ //origin
                    .x=(int)(parachute.width/2),
                    .y=(int)(parachute.height)
                },
                rotationParachute,
                WHITE
            ); 
        }
        
        //draw player
        DrawTexture(redSoldier.tx,redSoldier.x,redSoldier.y,WHITE);

        //draw rockets
        for(uint8_t i=0; i<numRockets; i++){
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
                    .x=(int)(rockets[i].tx.width/2),
                    .y=(int)(rockets[i].tx.height/2)
                },
                rockets[i].rotation,
                WHITE
            );
        }

        //draw rocket launcher
        DrawTexturePro(
            rl.tx,
            (Rectangle){ //src
                .x=0,
                .y=0,
                .width=rl.tx.width,
                .height=rl.flip*rl.tx.height
            },
            (Rectangle){ //dest
                .x=rl.x,
                .y=rl.y,
                .width=rl.tx.width,
                .height=rl.tx.height
            },
            (Vector2){ //origin
                .x=(int)(rl.tx.width/2),
                .y=(int)(rl.tx.height/2)
            },
            rl.rotation,
            rl.color
        ); 

        //update particles
        for(uint8_t i=0; i<numParticles; i++){
            if(redSoldier.y==(int)(SCREENHEIGHT/2)-(int)(redSoldier.tx.height/2) && redSoldier.speedY<0){
                particles[i].y-=redSoldier.speedY*dt;
            }  

            //fade away 
            particles[i].alpha-=2*dt;
            
            //draw
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
                    .x=(int)(particles[i].tx.width/2),
                    .y=(int)(particles[i].tx.height/2)
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
    
        //text
        switch(gameState){
            case 0: //game not started
                //bg
                DrawText( //TODO: TURN THESE INTO A FUNCTION, AUTOMATICALLY DRAW BACKGROUND AND FOREGROUND, BOOLEAN ARGUMENT CENTERED
                    "ROCKETMAN ADVENTURES", 
                    (int)(SCREENWIDTH/2)-(int)(MeasureTextEx(GetFontDefault(), "ROCKETMAN ADVENTURES", 100, 10).x/2),
                    200,
                    100,
                    BLACK
                );

                DrawText(
                    VERSION, 
                    (int)(SCREENWIDTH/2)-(int)(MeasureTextEx(GetFontDefault(), VERSION, 64, 10).x/2),
                    300,
                    64,
                    BLACK
                ); 

                DrawText(
                    "START JUMPING TO BEGIN", 
                    (int)(SCREENWIDTH/2)-(int)(MeasureTextEx(GetFontDefault(), "START JUMPING TO BEGIN", 64, 10).x/2),
                    400,
                    64,
                    BLACK
                );

                //fg
                DrawText(
                    "ROCKETMAN ADVENTURES", 
                    (int)(SCREENWIDTH/2)-(int)(MeasureTextEx(GetFontDefault(), "ROCKETMAN ADVENTURES", 100, 10).x/2)+7,
                    200+7,
                    100,
                    WHITE
                );

                DrawText(
                    VERSION, 
                    (int)(SCREENWIDTH/2)-(int)(MeasureTextEx(GetFontDefault(), VERSION, 64, 10).x/2)+7,
                    300+7,
                    64,
                    WHITE
                ); 

                DrawText(
                    "START JUMPING TO BEGIN", 
                    (int)(SCREENWIDTH/2)-(int)(MeasureTextEx(GetFontDefault(), "START JUMPING TO BEGIN", 64, 10).x/2)+7,
                    400+7,
                    64,
                    WHITE
                ); 
                break;
            case 1: //game in progress
                //bg
                DrawText("SCORE:", 10, 10, 64, BLACK);
                DrawText(scoreString,250, 10, 64, BLACK);
                
                //fg
                DrawText("SCORE:", 10+7, 10+7, 64, WHITE);
                DrawText(scoreString,250+7, 10+7, 64, WHITE); 
                break;
            case 2: //game over
                DrawRectangle(0,0,SCREENWIDTH,SCREENHEIGHT,(Color){0,0,0,150});
               
                //bg
                DrawText(
                    "GAME OVER", 
                    (int)(SCREENWIDTH/2)-(int)(MeasureTextEx(GetFontDefault(), "GAME OVER", 100, 10).x/2),
                    (int)(SCREENHEIGHT/2)-(int)(MeasureTextEx(GetFontDefault(), "GAME OVER", 100, 10).y/2)-100,
                    100,
                    BLACK
                );
                DrawText(
                    "SCORE:", 
                    (int)(SCREENWIDTH/2)-(int)(MeasureTextEx(GetFontDefault(), "SCORE:", 64, 10).x/2),
                    (int)(SCREENHEIGHT/2)-(int)(MeasureTextEx(GetFontDefault(), "SCORE", 64, 10).y/2),
                    64,
                    BLACK
                );
                DrawText(
                    scoreString, 
                    (int)(SCREENWIDTH/2)-(int)(MeasureTextEx(GetFontDefault(), scoreString, 64, 10).x/2),
                    (int)(SCREENHEIGHT/2)-(int)(MeasureTextEx(GetFontDefault(), scoreString, 64, 10).y/2)+100,
                    64,
                    BLACK
                );

                //fg
                DrawText(
                    "GAME OVER", 
                    (int)(SCREENWIDTH/2)-(int)(MeasureTextEx(GetFontDefault(), "GAME OVER", 100, 10).x/2)+7,
                    (int)(SCREENHEIGHT/2)-(int)(MeasureTextEx(GetFontDefault(), "GAME OVER", 100, 10).y/2)-100+7,
                    100,
                    WHITE
                );
                DrawText(
                    "SCORE:", 
                    (int)(SCREENWIDTH/2)-(int)(MeasureTextEx(GetFontDefault(), "SCORE:", 64, 10).x/2)+7,
                    (int)(SCREENHEIGHT/2)-(int)(MeasureTextEx(GetFontDefault(), "SCORE", 64, 10).y/2)+7,
                    64,
                    WHITE
                );
                DrawText(
                    scoreString, 
                    (int)(SCREENWIDTH/2)-(int)(MeasureTextEx(GetFontDefault(), scoreString, 64, 10).x/2)+7,
                    (int)(SCREENHEIGHT/2)-(int)(MeasureTextEx(GetFontDefault(), scoreString, 64, 10).y/2)+100+7,
                    64,
                    WHITE
                ); 
                break;
            default:
                DrawText("ERROR", 100, 100, 120, BLACK);
        } 

        EndDrawing();
    }
   
    //unload images
    UnloadImage(Images.rocket);
    UnloadImage(Images.particleSmoke);
    UnloadImage(Images.parachutePickup);
    UnloadImage(Images.critPickup);

    //unload textures
    UnloadTexture(redSoldier.tx); 
    for(uint8_t i=0; i<numRockets; i++){
        UnloadTexture(rockets[i].tx); 
    } 
    for(uint8_t i=0; i<numParticles; i++){
        UnloadTexture(particles[i].tx);
    }
    for(uint8_t i=0; i<numPlatforms; i++){
        UnloadTexture(platforms[i].tx);
    }
    UnloadTexture(pickup.tx);
    UnloadTexture(parachute);

    CloseAudioDevice();
    CloseWindow();

    return 0;
}
