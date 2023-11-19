#include <raylib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "structs.h"
#include "functions.h"
#include "globals.h"

int main(void){
    InitWindow(SCREENWIDTH,SCREENHEIGHT,"Rocketman Adventures");
    
    ui8 display=GetCurrentMonitor();
    SetWindowSize(GetMonitorWidth(display),GetMonitorHeight(display));
    ToggleFullscreen();

    InitAudioDevice();

    SetTargetFPS(60);
    float dt=1.f;

    ui8 gameState=0; //0 - not started, 1 - in progress, 2 - game over

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

    Images.hud=LoadImage(pathToFile("hud.png"));
    ImageResizeNN(&Images.hud,64*5,32*5);

    //sfx
    Sound fxExplosion=LoadSound(pathToFile("explosion.ogg"));
    Sound fxPickup=LoadSound(pathToFile("pickup.ogg"));
    Music musicMenu=LoadMusicStream(pathToFile("soundtrack3.ogg"));
    Music music=LoadMusicStream(pathToFile("soundtrack0.ogg"));

    //player
    Soldier redSoldier={
        .tx=LoadTextureFromImage(Images.redSoldier),
        .speedX=0,
        .speedY=0,
        .cooldown=0,
        .falling=0,
        .slowfall=1,
        .critBoost=1,
        .hp=200,
    };
    redSoldier.x=(int)(SCREENWIDTH/2)-redSoldier.tx.width;
    redSoldier.y=SCREENHEIGHT-redSoldier.tx.height;
    UnloadImage(Images.redSoldier);  

    //parachute
    Texture parachute=LoadTextureFromImage(Images.parachute);
    UnloadImage(Images.parachute);
    i8 rotationParachute=0; 

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
    for(ui8 i=0; i<2; i++){
        backgrounds[i]=LoadTextureFromImage(Images.backgrounds[i]);
        UnloadImage(Images.backgrounds[i]);
        bgY[i]=-i*SCREENHEIGHT;
    }

    srand(time(NULL));

    ui8 numRockets=0;
    Rocket* rockets=malloc(numRockets*sizeof(Rocket));

    ui8 numParticles=0;
    Particle* particles=malloc(numParticles*sizeof(Particle));

    ui8 numPlatforms=10;
    Platform platforms[numPlatforms];
    for(ui8 i=0; i<numPlatforms; i++){
        Platform newPlatform={
            .tx=LoadTextureFromImage(Images.platform),
            .x=rand()%SCREENWIDTH, //this is also used for random x when moving platform to the top
            .y=SCREENHEIGHT-(i+1)*100
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

    us score=0;
    char scoreString[5];

    //HUD
    HUD healthHUD={
        .tx=LoadTextureFromImage(Images.hud),
        .x=5,
        .y=SCREENHEIGHT-Images.hud.height-5,
        .text="0"
    };
    UnloadImage(Images.hud);
    sprintf(healthHUD.text, "%u", redSoldier.hp);

    PlayMusicStream(music);
    PlayMusicStream(musicMenu);   

    //game loop
    while(!WindowShouldClose()){
        dt=GetFrameTime();

        for(ui8 i=0; i<numRockets; i++){
            rocketBorderCheck(&rockets[i]);

            if(rockets[i].collided){
                PlaySound(fxExplosion);
                UnloadTexture(rockets[i].tx);

                //smoke particles
                if(rockets[i].shouldExplode){
                    for(ui8 j=0; j<3; j++){
                        numParticles++;
                        Particle *buffer=malloc(sizeof(Particle)*numParticles);
                    
                        Particle newParticle={
                            .tx=LoadTextureFromImage(Images.particleSmoke),
                            .x=rockets[i].x+MIDDLEX(rockets[i])+rand()%(50-(-50)+1)-50,
                            .y=rockets[i].y+MIDDLEY(rockets[i])+rand()%(50-(-50)+1)-50,
                            .rotation=rand()%361,
                            .alpha=255
                        };

                        for(ui8 i=0; i<numParticles-1; i++)
                            buffer[i]=particles[i];

                        buffer[numParticles-1]=newParticle;
                        particles=buffer;
                    }

                    if(abs(redSoldier.x+MIDDLEX(redSoldier)-rockets[i].x-MIDDLEX(rockets[i]))<100 
                    && abs(redSoldier.y+MIDDLEY(redSoldier)-rockets[i].y-MIDDLEY(rockets[i]))<100
                    && gameState!=2){
                        //rocket jump
                        redSoldier.speedX=redSoldier.critBoost*-1*rockets[i].speedX;
                        redSoldier.speedY=redSoldier.critBoost*-1*rockets[i].speedY; 
                    
                        //damage
                        if(gameState==1){
                            redSoldier.hp-=20*redSoldier.critBoost;
                            if(redSoldier.hp<=0)
                                gameState=2;
                        }
                    }

                    if(redSoldier.pickupActive==2)
                        redSoldier.pickupActive=0;
                }

                //delete rockets
                numRockets--; 

                //shift elements in array
                for(ui8 j=i; j<numRockets; j++)
                    rockets[j]=rockets[j+1];
               
                //TEMPORARY SOLUTION 
                Rocket* buffer=malloc(sizeof(Rocket)*numRockets);
                for(ui8 j=0; j<numRockets; j++)
                    buffer[j]=rockets[j];
                rockets=buffer;

                break;
            }
        }

        //delete particles
        for(ui8 i=0; i<numParticles; i++){
            if(particles[i].alpha<5){
                numParticles--;

                //shift elements in array
                for(ui8 j=i; j<numParticles; j++)
                    particles[j]=particles[j+1];

                Particle* buffer=malloc(sizeof(Particle)*numParticles);
                for(ui8 j=0; j<numParticles; j++)
                    buffer[j]=particles[j];
                particles=buffer;

                break;
            }
        }

        //movement
        if(gameState!=2){
            if(IsKeyDown(KEY_D) && !IsKeyDown(KEY_A)){
                redSoldier.x+=150*dt;

                if(redSoldier.pickupActive==1 && rotationParachute>-30)
                    rotationParachute-=60*dt;
            }
            if(IsKeyDown(KEY_A) && !IsKeyDown(KEY_D)){
                redSoldier.x-=150*dt;
            
                if(redSoldier.pickupActive==1 && rotationParachute<30)
                    rotationParachute+=60*dt;
            }
            //reset parachute rotation
            if(!IsKeyDown(KEY_A) && !IsKeyDown(KEY_D)) //if not moving horizontally
                rotationParachute+=rotationParachute>0?-100*dt:100*dt;
            if(IsKeyDown(KEY_SPACE) && !redSoldier.falling){
                if(redSoldier.pickupActive==1)
                    redSoldier.pickupActive=0;
                redSoldier.falling=0;
                redSoldier.speedY=-300;
            }

            //update rocket launcher
            rl.x=redSoldier.x+MIDDLEX(redSoldier);
            rl.y=redSoldier.y+MIDDLEY(redSoldier); 
            rl.rotation=270-atan2((redSoldier.x+MIDDLEX(redSoldier)-GetMouseX()),(redSoldier.y+MIDDLEY(redSoldier)-GetMouseY()))*180/PI; 
        
            //flip the rocket launcher to prevent it from going upside down
            rl.flip=GetMouseX()<redSoldier.x+MIDDLEX(redSoldier)?-1:1;
        }

        //update player position
        redSoldier.x+=redSoldier.speedX*dt;
        if(redSoldier.speedY>0){
            if(redSoldier.falling && gameState!=2)
                redSoldier.y+=redSoldier.speedY*dt*redSoldier.slowfall; 
        }
        else
            redSoldier.y+=redSoldier.speedY*dt; 

        if(redSoldier.y<SCREENMIDDLE(redSoldier)){
            //score
            score-=redSoldier.speedY*dt;
            sprintf(scoreString, "%hu", score);

            redSoldier.y=SCREENMIDDLE(redSoldier); 
           
            if(gameState==0)
                gameState=1;
        }

        //gravity
        if(redSoldier.y+redSoldier.tx.height>=SCREENHEIGHT){
            if(gameState!=1){
                redSoldier.y=SCREENHEIGHT-redSoldier.tx.height;
                redSoldier.speedY=0;
                redSoldier.falling=0;
            }
            else gameState=2;
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
                .x=redSoldier.x+MIDDLEX(redSoldier),
                .y=redSoldier.y+MIDDLEY(redSoldier),
                .rotation=90-atan2((redSoldier.x+MIDDLEX(redSoldier)-GetMouseX()),(redSoldier.y+MIDDLEY(redSoldier)-GetMouseY()))*180/PI,
                .collided=0,
                .shouldExplode=1
            };
            
            newRocket.speedX=-1.2*cos(newRocket.rotation*PI/180)*800;
            newRocket.speedY=-1.2*sin(newRocket.rotation*PI/180)*800;
            
            for(ui8 i=0; i<numRockets-1; i++)
                buffer[i]=rockets[i];

            buffer[numRockets-1]=newRocket;
            rockets=buffer;
        }
        //ACTIVATE PICKUP
        if(IsKeyPressed(KEY_Q)){
            redSoldier.pickupActive=redSoldier.pickup;
            redSoldier.pickup=0;
        }
       
        //horizontal friction
        redSoldier.speedX+=redSoldier.speedX>0?-8:8;
        if(redSoldier.speedX>-5 && redSoldier.speedX<5)
            redSoldier.speedX=0;

        soldierBorderCheck(&redSoldier);

        //update cooldowns
        redSoldier.cooldown-=150*GetFrameTime();

        //update rockets
        for(ui8 i=0; i<numRockets; i++){
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
        for(ui8 i=0; i<2; i++){
            //parallax scrolling
            if(bgY[i]>SCREENHEIGHT){
                bgY[i]=-SCREENHEIGHT;
                bgY[1-i]=0;
            } 
            if(redSoldier.y==SCREENMIDDLE(redSoldier) && redSoldier.speedY<0)
                bgY[i]-=bgShift;

            //draw background
            DrawTexture(backgrounds[i],0,bgY[i],WHITE);
        }

        //update platforms
        for(ui8 i=0; i<numPlatforms; i++){
            //soldier collisions
            if(redSoldier.speedY>0)
                platformCollisionCheckS(&platforms[i],&redSoldier);

            if(redSoldier.y==SCREENMIDDLE(redSoldier) && redSoldier.speedY<0)
                platforms[i].y-=redSoldier.speedY*dt;

            //rocket collisions
            for(ui8 j=0; j<numRockets; j++)
                platformCollisionCheckR(&platforms[i],&rockets[j]);

            if(platforms[i].y>SCREENHEIGHT){
                platforms[i].x=rand()%SCREENWIDTH;
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
        if(redSoldier.y==SCREENMIDDLE(redSoldier) && redSoldier.speedY<0)
            pickup.y-=redSoldier.speedY*dt; 
        if(pickupVisible(&pickup))
            DrawTexture(pickup.tx,pickup.x,pickup.y,WHITE);

        //parachute
        if(redSoldier.slowfall<1){
            DrawTexturePro(
                parachute,
                (Rectangle){ //src
                    .x=0,
                    .y=0,
                    .width=parachute.width,
                    .height=parachute.height
                },
                (Rectangle){ //dest
                    .x=redSoldier.x+MIDDLEX(redSoldier),
                    .y=redSoldier.y,
                    .width=parachute.width,
                    .height=parachute.height
                },
                (Vector2){ //origin
                    .x=(int)(parachute.width/2),
                    .y=parachute.height
                },
                rotationParachute,
                WHITE
            ); 
        }
        
        //draw player
        DrawTexture(redSoldier.tx,redSoldier.x,redSoldier.y,WHITE);

        //draw rockets
        for(ui8 i=0; i<numRockets; i++){
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
                    .x=MIDDLEX(rockets[i]),
                    .y=MIDDLEY(rockets[i])
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
                .x=MIDDLEX(rl),
                .y=MIDDLEY(rl)
            },
            rl.rotation,
            rl.color
        ); 

        //update particles
        for(ui8 i=0; i<numParticles; i++){
            if(redSoldier.y==SCREENMIDDLE(redSoldier) && redSoldier.speedY<0)
                particles[i].y-=redSoldier.speedY*dt;

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
                    .x=MIDDLEX(particles[i]),
                    .y=MIDDLEY(particles[i])
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
    
        //text and hud
        switch(gameState){
            case 0: //game not started
                UpdateMusicStream(musicMenu);

                drawTextFullCenter("ROCKETMAN ADVENTURES",200, 100);
                drawTextFullCenter(VERSION, 300,64); 
                drawTextFullCenter("START JUMPING TO BEGIN",400,64);
                break;
            case 1: //game in progress
                UpdateMusicStream(music);

                sprintf(healthHUD.text,"%u",redSoldier.hp);
                DrawTexture(healthHUD.tx,healthHUD.x,healthHUD.y,WHITE);
                drawTextFull(healthHUD.text,healthHUD.x+40,healthHUD.y+30,100); 
                
                drawTextFull("SCORE:", 10, 10, 64);
                drawTextFull(scoreString,250, 10, 64);
                break;
            case 2: //game over
                DrawRectangle(0,0,SCREENWIDTH,SCREENHEIGHT,(Color){0,0,0,150});
                drawTextFullCenter("GAME OVER",200,100);
                drawTextFullCenter("SCORE:",300,64);
                drawTextFullCenter(scoreString,375,64);
                break;
            default:
                drawTextFull("ERROR", 100, 100, 120);
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
    for(ui8 i=0; i<numRockets; i++)
        UnloadTexture(rockets[i].tx); 
    for(ui8 i=0; i<numParticles; i++)
        UnloadTexture(particles[i].tx);
    for(ui8 i=0; i<numPlatforms; i++)
        UnloadTexture(platforms[i].tx);
    UnloadTexture(pickup.tx);
    UnloadTexture(parachute);
    UnloadTexture(healthHUD.tx);

    CloseAudioDevice();
    CloseWindow();

    return 0;
}
