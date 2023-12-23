#include <raylib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

#include "structs.h"
#include "functions.h"
#include "globals.h"
#include "config.h"

int main(void){
    InitWindow(SCREENWIDTH,SCREENHEIGHT,"Rocketman Adventures");
    
    int display=GetCurrentMonitor();
    SetWindowSize(GetMonitorWidth(display),GetMonitorHeight(display));
    ToggleFullscreen();

    InitAudioDevice();

    SetTargetFPS(FPS);
    float dt=1.f;

    SetMasterVolume((float)volume/100);

    //load and resize images
    Images.redSoldier=LoadImage(pathToFile("red_soldier.png"));
    ImageResizeNN(&Images.redSoldier,12*5,20*5);

    Images.rocket=LoadImage(pathToFile("rocket.png"));
    ImageResizeNN(&Images.rocket,30*3,8*3);

    Images.launcher=LoadImage(pathToFile("launcher.png"));
    ImageResizeNN(&Images.launcher,20*5,8*5);

    Images.particleSmoke=LoadImage(pathToFile("particle_smoke.png"));
    ImageResizeNN(&Images.particleSmoke,12*12,12*12);

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

    Images.healthPack=LoadImage(pathToFile("health_pack.png"));
    ImageResizeNN(&Images.healthPack,12*6,12*6);

    Images.button[0]=LoadImage(pathToFile("button_normal.png"));
    ImageResizeNN(&Images.button[0], 50*8, 20*8);

    Images.button[1]=LoadImage(pathToFile("button_hover.png"));
    ImageResizeNN(&Images.button[1], 50*8, 20*8);

    //BACKGROUNDS
    Images.bgForest=LoadImage(pathToFile("bg_forest.png"));
    ImageResizeNN(&Images.bgForest,SCREENWIDTH,SCREENHEIGHT);

    for(int i=0; i<3; i++){
        char name[16]="bg_sky";
        char num[2];
        sprintf(num,"%d",i);
        strcat(name,num);
        strcat(name,".png");
        Images.bgSky[i]=LoadImage(pathToFile(name));
        ImageResizeNN(&Images.bgSky[i],SCREENWIDTH,SCREENHEIGHT);
    }

    Images.bgSkyStars=LoadImage(pathToFile("bg_sky_stars.png"));
    ImageResizeNN(&Images.bgSkyStars,SCREENWIDTH,SCREENHEIGHT);

    Images.bgStars=LoadImage(pathToFile("bg_stars.png"));
    ImageResizeNN(&Images.bgStars,SCREENWIDTH,SCREENHEIGHT); 

    Images.bgStarsSpace=LoadImage(pathToFile("bg_stars_space.png"));
    ImageResizeNN(&Images.bgStarsSpace,SCREENWIDTH,SCREENHEIGHT);

    for(int i=0; i<2; i++){
        char name[16]="bg_space";
        char num[2];
        sprintf(num,"%d",i);
        strcat(name,num);
        strcat(name,".png");
        Images.bgSpace[i]=LoadImage(pathToFile(name));
        ImageResizeNN(&Images.bgSpace[i],SCREENWIDTH,SCREENHEIGHT);
    }  

    Texture bgTxs[17];
    bgTxs[0]=LoadTextureFromImage(Images.bgForest);
    for(int i=1; i<10; i++)
        bgTxs[i]=LoadTextureFromImage(Images.bgSky[(i-1)%3]);
    bgTxs[10]=LoadTextureFromImage(Images.bgSkyStars);
    for(int i=11; i<14; i++)
        bgTxs[i]=LoadTextureFromImage(Images.bgStars);
    bgTxs[14]=LoadTextureFromImage(Images.bgStarsSpace);
    for(int i=15; i<17; i++)
        bgTxs[i]=LoadTextureFromImage(Images.bgSpace[(i-1)%2]);

    //UNLOAD BACKGROUNDS
    UnloadImage(Images.bgForest);
    for(int i=0; i<3; i++)
        UnloadImage(Images.bgSky[i]);
    UnloadImage(Images.bgSkyStars);
    UnloadImage(Images.bgStars);
    UnloadImage(Images.bgStarsSpace);
    for(int i=0; i<2; i++)
        UnloadImage(Images.bgSpace[i]);

    //sfx
    Sound fxExplosion=LoadSound(pathToFile("explosion.ogg"));
    Sound fxPickup=LoadSound(pathToFile("pickup.ogg"));
    Music musicMenu=LoadMusicStream(pathToFile("soundtrack_menu.ogg"));
    Music musicNormal=LoadMusicStream(pathToFile("soundtrack_normal.ogg"));
    Music musicSpace=LoadMusicStream(pathToFile("soundtrack_space.ogg"));

    int gameState; //0 - not started, 1 - in progress, 2 - game over

    //player
    Soldier redSoldier={
        .tx=LoadTextureFromImage(Images.redSoldier), 
    }; 
    UnloadImage(Images.redSoldier);  

    //parachute
    Texture parachute=LoadTextureFromImage(Images.parachute);
    UnloadImage(Images.parachute);
    int rotationParachute=0; 

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
    Texture bgs[2];
    int bgY[2];  
    int level; //used for changing backgrounds

    srand(time(NULL));

    Rocket newRocket={
        .tx=LoadTextureFromImage(Images.rocket),
        .x=-100,
        .y=-100,
        .speedX=0,
        .speedY=0,
        .rotation=0,
        .collided=0,
        .shouldExplode=1,
        .isFree=1
    };
    Rocket rockets[MAXROCKETS]; 
    UnloadImage(Images.rocket);       

    Particle newParticle={
        .tx=LoadTextureFromImage(Images.particleSmoke),
        .x=-100,
        .y=-100,
        .rotation=0,
        .alpha=255,
        .isFree=1
    };
    Particle particles[MAXPARTICLES];
    UnloadImage(Images.particleSmoke);

    Platform platforms[NUM_PLATFORMS];
    for(int i=0; i<NUM_PLATFORMS; i++){
        Platform newPlatform={
            .tx=LoadTextureFromImage(Images.platform),
        };
        platforms[i]=newPlatform;
    }
    UnloadImage(Images.platform);
    
    int shift=0;

    //pickups
    Pickup pickup={
        .txs[0]=LoadTextureFromImage(Images.parachutePickup),
        .txs[1]=LoadTextureFromImage(Images.critPickup),
        .tx=LoadTextureFromImage(Images.parachutePickup), 
    };
    UnloadImage(Images.parachutePickup);
    UnloadImage(Images.critPickup);

    //health packs
    HealthPack healthPacks[NUM_HEALTHPACKS];
    for(int i=0; i<NUM_HEALTHPACKS; i++){
        HealthPack newHealthPack={
            .tx=LoadTextureFromImage(Images.healthPack),
        };
        healthPacks[i]=newHealthPack=newHealthPack;
    }
    UnloadImage(Images.healthPack);

    int score;
    char scoreString[5];

    //hp hud
    HUD healthHUD={
        .tx=LoadTextureFromImage(Images.hud),
        .x=5,
        .y=SCREENHEIGHT-Images.hud.height-5,
        .text="0"
    }; 
    sprintf(healthHUD.text, "%u", redSoldier.hp);

    //pickup hud
    HUD pickupHUD={
        .tx=LoadTextureFromImage(Images.hud),
        .x=SCREENWIDTH-Images.hud.width-5,
        .y=SCREENHEIGHT-Images.hud.height-5,
        .text="EMPTY"
    };
    UnloadImage(Images.hud); 

    //buttons
    Button tryAgainButton={
        .tx[0]=LoadTextureFromImage(Images.button[0]),
        .tx[1]=LoadTextureFromImage(Images.button[1]),
        .x=SCREENWIDTH/2-Images.button[0].width/2,
        .y=500,
        .text="TRY AGAIN"
    };
    for(int i=0; i<2; i++)
        UnloadImage(Images.button[i]);
    
    Vector2 mouse;
    
    START:
    gameState=0;
    
    level=1;
    bgs[0]=bgTxs[0];
    bgs[1]=bgTxs[1];

    redSoldier.x=(int)(SCREENWIDTH/2)-redSoldier.tx.width;
    redSoldier.y=SCREENHEIGHT-redSoldier.tx.height; 
    redSoldier.speedX=0;
    redSoldier.speedY=0;
    redSoldier.cooldown=0;
    redSoldier.falling=0;
    redSoldier.slowfall=1;
    redSoldier.critBoost=1;
    redSoldier.hp=200;

    for(int i=0; i<2; i++)
        bgY[i]=-i*SCREENHEIGHT;

    for(int i=0; i<MAXROCKETS; i++)
        rockets[i]=newRocket;  

    for(int i=0; i<MAXPARTICLES; i++)
        particles[i]=newParticle;

    for(int i=0; i<NUM_PLATFORMS; i++){
        platforms[i].x=rand()%(SCREENWIDTH-Images.platform.width-400)+200; //this is also used for random x when moving platform to the top
        platforms[i].y=SCREENHEIGHT-(i+1)*1000/NUM_PLATFORMS;
    } 

    pickup.x=-100;
    pickup.y=-100;
    pickup.id=1;    

    for(int i=0; i<NUM_HEALTHPACKS; i++){
        healthPacks[i].x=-100;
        healthPacks[i].y=-100;
    }

    score=0;

    PlayMusicStream(musicNormal);
    PlayMusicStream(musicMenu);   
    PlayMusicStream(musicSpace);

    //game loop
    while(!WindowShouldClose()){
        dt=GetFrameTime();
        mouse=GetMousePosition();

        //volume control
        if(IsKeyPressed(VOLUP) && volume<=95){
            volume+=10;
            SetMasterVolume((float)volume/100);
        }
        else if(IsKeyPressed(VOLDOWN) && volume>=5){
            volume-=10;
            SetMasterVolume((float)volume/100);
        }
        else if(IsKeyPressed(MUTE)){
            muted=!muted;
            SetMasterVolume(muted?0:(float)volume/100);
        }

        for(int i=0; i<MAXROCKETS; i++){
            if(rockets[i].isFree) continue;
            rocketBorderCheck(&rockets[i]);

            if(rockets[i].collided){
                PlaySound(fxExplosion);

                //smoke particles
                if(rockets[i].shouldExplode){
                    for(int j=0; j<MAXPARTICLES; j++){
                        if(particles[j].isFree){
                            particles[j].x=rockets[i].x;
                            particles[j].y=rockets[i].y;
                            particles[j].rotation=rand()%360;
                            particles[j].isFree=0;
                            break;
                        }
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
                            if(redSoldier.hp<=0){
                                gameState=2;
                                
                                //reset soundtrack - same thing happens when player hits the ground
                                SeekMusicStream(musicNormal,0);
                                SeekMusicStream(musicMenu,0);
                                SeekMusicStream(musicSpace,0);
                            }
                        }
                    }
                    
                    if(redSoldier.pickupActive==2){
                        redSoldier.critBoost=1;
                        rl.color=WHITE;
                        redSoldier.pickupActive=0;
                    }
                }
                
                //prepare for shooting again
                rockets[i]=newRocket; 
            }
        }

        //prepare particles for future use
        for(int i=0; i<MAXPARTICLES; i++)
            if(!particles[i].isFree && particles[i].alpha<5)
                particles[i]=newParticle;
 
        if(gameState!=2){
            //movement 
            if(IsKeyDown(MOVERIGHT) && !IsKeyDown(MOVELEFT)){
                redSoldier.x+=150*dt;

                if(redSoldier.pickupActive==1 && rotationParachute>-30)
                    rotationParachute-=60*dt;
            }
            if(IsKeyDown(MOVELEFT) && !IsKeyDown(MOVERIGHT)){
                redSoldier.x-=150*dt;
            
                if(redSoldier.pickupActive==1 && rotationParachute<30)
                    rotationParachute+=60*dt;
            }
            //reset parachute rotation
            if(!IsKeyDown(MOVELEFT) && !IsKeyDown(MOVERIGHT)) //if not moving horizontally
                rotationParachute+=rotationParachute>0?-100*dt:100*dt;
            if(IsKeyDown(JUMP) && !redSoldier.falling){
                if(redSoldier.pickupActive==1){
                    redSoldier.slowfall=1;                     
                    redSoldier.pickupActive=0;
                }
                redSoldier.falling=0;
                redSoldier.speedY=-400;
            }

            //update rocket launcher
            rl.x=redSoldier.x+MIDDLEX(redSoldier);
            rl.y=redSoldier.y+MIDDLEY(redSoldier); 
            rl.rotation=270-atan2((redSoldier.x+MIDDLEX(redSoldier)-mouse.x),(redSoldier.y+MIDDLEY(redSoldier)-mouse.y))*180/PI; 
        
            //flip the rocket launcher to prevent it from going upside down
            rl.flip=mouse.x<redSoldier.x+MIDDLEX(redSoldier)?-1:1;
            
            //update player position
            redSoldier.x+=redSoldier.speedX*dt;
            if(redSoldier.speedY>0 && redSoldier.falling)
                redSoldier.y+=redSoldier.speedY*dt*redSoldier.slowfall; 
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
                else{
                    gameState=2;
            
                    //reset soundtrack
                    SeekMusicStream(musicNormal,0);
                    SeekMusicStream(musicMenu,0);
                    SeekMusicStream(musicSpace,0);
                }
            }
            else{
                redSoldier.falling=1;
                redSoldier.speedY+=1000*dt;
            } 
        
            if((IsMouseButtonPressed(SHOOT) || IsKeyPressed(SHOOT_ALT)) && redSoldier.cooldown<0){
                redSoldier.cooldown=120;

                for(int i=0; i<MAXROCKETS; i++){
                    if(rockets[i].isFree){
                        rockets[i].isFree=0;
                        rockets[i].x=redSoldier.x+MIDDLEX(rockets[i]);
                        rockets[i].y=redSoldier.y+MIDDLEY(rockets[i]);
                        rockets[i].rotation=90-atan2((redSoldier.x+MIDDLEX(redSoldier)-mouse.x),(redSoldier.y+MIDDLEY(redSoldier)-mouse.y))*180/PI;
                        rockets[i].speedX=-1.2*cos(rockets[i].rotation*PI/180)*800;
                        rockets[i].speedY=-1.2*sin(rockets[i].rotation*PI/180)*800; 
                        break;
                    }
                }   
            }

            //ACTIVATE PICKUP
            if(IsKeyPressed(USEPICKUP)){
                redSoldier.pickupActive=redSoldier.pickup;
                redSoldier.pickup=0;
                switch(redSoldier.pickupActive){
                    case 1:
                        redSoldier.slowfall=0.2;
                    break;

                    case 2:
                        redSoldier.critBoost=2;
                        rl.color=RED;    
                    break; 
                }
            }
       
            //horizontal friction
            redSoldier.speedX+=redSoldier.speedX>0?-8:8;
            if(redSoldier.speedX>-5 && redSoldier.speedX<5)
                redSoldier.speedX=0;

            soldierBorderCheck(&redSoldier);
    
            //update cooldowns
            redSoldier.cooldown-=150*GetFrameTime();

            //update rockets
            for(int i=0; i<MAXROCKETS; i++){
                //position
                rockets[i].x+=rockets[i].speedX*dt;
                rockets[i].y+=rockets[i].speedY*dt;
            }  
        } 

        shift=redSoldier.speedY*dt;

        ClearBackground(BLACK); 
        BeginDrawing();

        //update background
        for(int i=0; i<2; i++){
            //parallax scrolling
            if(bgY[i]>SCREENHEIGHT){
                bgY[i]=-SCREENHEIGHT;
                bgY[1-i]=0;

                level++;
                if(level>16) level=16;
                bgs[i]=bgTxs[level];
            } 
            if(redSoldier.y==SCREENMIDDLE(redSoldier) && redSoldier.speedY<0)
                bgY[i]-=shift/2;

            //draw background
            DrawTexture(bgs[i],0,bgY[i],WHITE);
        }

        //update platforms
        for(int i=0; i<NUM_PLATFORMS; i++){
            //soldier collisions
            if(redSoldier.speedY>0)
                platformCollisionCheckS(&platforms[i],&redSoldier);

            if(redSoldier.y==SCREENMIDDLE(redSoldier) && redSoldier.speedY<0)
                platforms[i].y-=shift;

            //rocket collisions
            for(int j=0; j<MAXROCKETS; j++)
                platformCollisionCheckR(&platforms[i],&rockets[j]);

            if(platforms[i].y>SCREENHEIGHT){
                platforms[i].x=rand()%(SCREENWIDTH-platforms[i].tx.width-400)+200;
                platforms[i].y=-platforms[i].tx.height;
                
                //random pickups and health packs
                int random=rand()%10;
                if(random==0 && !VISIBLE(pickup)){ 
                    pickup.id=rand()%2+1;
                    pickup.tx=pickup.txs[pickup.id-1];
                    pickup.x=platforms[i].x+MIDDLEX(platforms[i])-MIDDLEX(pickup); 
                    pickup.y=platforms[i].y-pickup.tx.height; 
                }
                else if(random>7){
                    for(int j=0; j<NUM_HEALTHPACKS; j++){
                        if(!VISIBLE(healthPacks[j])){
                            healthPacks[j].x=platforms[i].x+MIDDLEX(platforms[i])-MIDDLEX(healthPacks[j]);
                            healthPacks[j].y=platforms[i].y-healthPacks[j].tx.height;
                            break;
                        }
                    }
                }
            }

            //draw platforms
            DrawTexture(platforms[i].tx,platforms[i].x,platforms[i].y,WHITE);
        }

        //update pickup
        if(pickupCollectCheck(&pickup, &redSoldier)) PlaySound(fxPickup); 
        if(redSoldier.y==SCREENMIDDLE(redSoldier) && redSoldier.speedY<0)
            pickup.y-=shift; 
        if(VISIBLE(pickup))
            DrawTexture(pickup.tx,pickup.x,pickup.y,WHITE);

        //update health packs
        for(int i=0; i<NUM_HEALTHPACKS; i++){
            if(VISIBLE(healthPacks[i])){
                DrawTexture(healthPacks[i].tx,healthPacks[i].x,healthPacks[i].y,WHITE);
                if(COLLISION(healthPacks[i],redSoldier)){
                    redSoldier.hp+=50;
                    healthPacks[i].x=-100;
                    healthPacks[i].y=-100;
                }
            }
            if(redSoldier.y==SCREENMIDDLE(redSoldier) && redSoldier.speedY<0)
                healthPacks[i].y-=shift;     
        }

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
        for(int i=0; i<MAXROCKETS; i++){
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
        for(int i=0; i<MAXPARTICLES; i++){
            if(!particles[i].isFree){
                if(redSoldier.y==SCREENMIDDLE(redSoldier) && redSoldier.speedY<0)
                    particles[i].y-=shift;

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
        } 
   
        //text and hud
        switch(gameState){
            case 0: //game not started
                UpdateMusicStream(musicMenu);

                drawTextFullCenter("ROCKETMAN ADVENTURES",200, 100, WHITE);
                drawTextFullCenter(VERSION, 300,64, WHITE); 
                drawTextFullCenter("START JUMPING TO BEGIN",400,64, WHITE);
                break;
            case 1: //game in progress
                if(level<15)
                    UpdateMusicStream(musicNormal);
                else
                    UpdateMusicStream(musicSpace);

                if(redSoldier.hp<50)
                    healthHUD.textColor=TEXTCOLOR[0];
                else if(redSoldier.hp>200)
                    healthHUD.textColor=TEXTCOLOR[2];
                else
                    healthHUD.textColor=TEXTCOLOR[1];

                //hp hud
                sprintf(healthHUD.text,"%u",redSoldier.hp);
                DrawTexture(healthHUD.tx,healthHUD.x,healthHUD.y,WHITE);
                drawTextFull(healthHUD.text,healthHUD.x+40,healthHUD.y+30,100, healthHUD.textColor); 
               
                //pickup hud
                DrawTexturePro(
                    pickupHUD.tx,
                    (Rectangle){ //src
                        .x=0,
                        .y=0,
                        .width=-1*pickupHUD.tx.width,
                        .height=pickupHUD.tx.height
                    },
                    (Rectangle){ //dest
                        .x=pickupHUD.x,
                        .y=pickupHUD.y,
                        .width=pickupHUD.tx.width,
                        .height=pickupHUD.tx.height
                    },
                    (Vector2){ //origin
                        .x=0,
                        .y=0
                    },
                    0,
                    WHITE
                );
                if(redSoldier.pickup==1 || redSoldier.pickup==2)
                    DrawTexture(pickup.txs[redSoldier.pickup-1],pickupHUD.x+150, pickupHUD.y+25, WHITE);
                else
                    drawTextFull(pickupHUD.text,pickupHUD.x+65,pickupHUD.y+40,64,WHITE);

                //score
                drawTextFull("SCORE:", 10, 10, 64, WHITE);
                drawTextFull(scoreString,250, 10, 64, WHITE);
                break;
            case 2: //game over
                //update buttons
                if(MOUSEHOVERBUTTON(tryAgainButton,mouse)){
                    tryAgainButton.state=1;
                    if(IsMouseButtonPressed(SHOOT))
                        goto START;
                }
                else
                    tryAgainButton.state=0;

                DrawRectangle(0,0,SCREENWIDTH,SCREENHEIGHT,(Color){0,0,0,150});
                drawTextFullCenter("GAME OVER",200,100, WHITE);
                drawTextFullCenter("SCORE:",300,64, WHITE);
                drawTextFullCenter(scoreString,375,64, WHITE);
                DrawTexture(tryAgainButton.tx[tryAgainButton.state],tryAgainButton.x,tryAgainButton.y,WHITE);
                drawTextFull(tryAgainButton.text, tryAgainButton.x+12, tryAgainButton.y+45, 64, WHITE);
                break;
            default:
                drawTextFull("ERROR", 100, 100, 120, WHITE);
        } 

        EndDrawing();
    }
   
    //unload textures
    UnloadTexture(redSoldier.tx); 
    for(int i=0; i<MAXROCKETS; i++)
        UnloadTexture(rockets[i].tx); 
    for(int i=0; i<MAXPARTICLES; i++)
        UnloadTexture(particles[i].tx);
    for(int i=0; i<NUM_PLATFORMS; i++)
        UnloadTexture(platforms[i].tx);
    for(int i=0; i<17; i++)
        UnloadTexture(bgTxs[i]);
    for(int i=0; i<2; i++){
        UnloadTexture(bgs[i]);
        UnloadTexture(tryAgainButton.tx[i]);
    }
    for(int i=0; i<NUM_HEALTHPACKS; i++){
        UnloadTexture(healthPacks[i].tx);
    }
    UnloadTexture(pickup.tx);
    UnloadTexture(parachute);
    UnloadTexture(healthHUD.tx);
    UnloadTexture(pickupHUD.tx);

    //unload music
    UnloadMusicStream(musicMenu);
    UnloadMusicStream(musicNormal);
    UnloadMusicStream(musicSpace);

    CloseAudioDevice();
    CloseWindow();

    return 0;
}
