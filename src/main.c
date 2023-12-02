#include <raylib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "structs.h"
#include "functions.h"
#include "globals.h"
#include "config.h"

int main(void){
    InitWindow(SCREENWIDTH,SCREENHEIGHT,"Rocketman Adventures");
    
    ui8 display=GetCurrentMonitor();
    SetWindowSize(GetMonitorWidth(display),GetMonitorHeight(display));
    ToggleFullscreen();

    InitAudioDevice();

    SetTargetFPS(FPS);
    float dt=1.f;

    SetMasterVolume((float)volume/100);

    //load and resize images
    Images.redSoldier=LoadImage(PATH("red_soldier.png"));
    ImageResizeNN(&Images.redSoldier,12*5,20*5);

    Images.rocket=LoadImage(PATH("rocket.png"));
    ImageResizeNN(&Images.rocket,30*3,8*3);

    Images.launcher=LoadImage(PATH("launcher.png"));
    ImageResizeNN(&Images.launcher,20*5,8*5);

    Images.particleSmoke=LoadImage(PATH("particle_smoke.png"));
    ImageResizeNN(&Images.particleSmoke,12*10,12*10);

    Images.backgrounds[0]=LoadImage(PATH("background0.png"));
    ImageResizeNN(&Images.backgrounds[0],SCREENWIDTH,SCREENHEIGHT);

    Images.backgrounds[1]=LoadImage(PATH("background1.png"));
    ImageResizeNN(&Images.backgrounds[1],SCREENWIDTH,SCREENHEIGHT);

    Images.platform=LoadImage(PATH("platform.png"));
    ImageResizeNN(&Images.platform,30*5,2*5);

    Images.parachutePickup=LoadImage(PATH("parachute_pickup.png"));
    ImageResizeNN(&Images.parachutePickup, 16*4, 20*4);

    Images.parachute=LoadImage(PATH("parachute.png"));
    ImageResizeNN(&Images.parachute, 13*8, 11*8);

    Images.critPickup=LoadImage(PATH("crit_pickup.png"));
    ImageResizeNN(&Images.critPickup,9*8,13*8);

    Images.hud=LoadImage(PATH("hud.png"));
    ImageResizeNN(&Images.hud,64*5,32*5);

    Images.healthPack=LoadImage(PATH("health_pack.png"));
    ImageResizeNN(&Images.healthPack,12*6,12*6);

    Images.button[0]=LoadImage(PATH("button_normal.png"));
    ImageResizeNN(&Images.button[0], 50*8, 20*8);

    Images.button[1]=LoadImage(PATH("button_hover.png"));
    ImageResizeNN(&Images.button[1], 50*8, 20*8);

    //sfx
    Sound fxExplosion=LoadSound(PATH("explosion.ogg"));
    Sound fxPickup=LoadSound(PATH("pickup.ogg"));
    Music musicMenu=LoadMusicStream(PATH("soundtrack3.ogg"));
    Music music=LoadMusicStream(PATH("soundtrack0.ogg"));

    ui8 gameState; //0 - not started, 1 - in progress, 2 - game over

    //player
    Soldier redSoldier={
        .tx=LoadTextureFromImage(Images.redSoldier), 
    }; 
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
    }

    srand(time(NULL));

    ui8 numRockets;
    Rocket* rockets;

    ui8 numParticles;
    Particle* particles;

    Platform platforms[NUM_PLATFORMS];
    for(ui8 i=0; i<NUM_PLATFORMS; i++){
        Platform newPlatform={
            .tx=LoadTextureFromImage(Images.platform),
        };
        platforms[i]=newPlatform;
    }
    UnloadImage(Images.platform);
    
    short shift=0;

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
    for(ui8 i=0; i<NUM_HEALTHPACKS; i++){
        HealthPack newHealthPack={
            .tx=LoadTextureFromImage(Images.healthPack),
        };
        healthPacks[i]=newHealthPack=newHealthPack;
    }
    UnloadImage(Images.healthPack);

    us score;
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
    for(ui8 i=0; i<2; i++)
        UnloadImage(Images.button[i]);
    
    Vector2 mouse;
    
    START:
    gameState=0;
   
    redSoldier.x=(int)(SCREENWIDTH/2)-redSoldier.tx.width;
    redSoldier.y=SCREENHEIGHT-redSoldier.tx.height; 
    redSoldier.speedX=0;
    redSoldier.speedY=0;
    redSoldier.cooldown=0;
    redSoldier.falling=0;
    redSoldier.slowfall=1;
    redSoldier.critBoost=1;
    redSoldier.hp=200;

    for(ui8 i=0; i<2; i++)
        bgY[i]=-i*SCREENHEIGHT;

    numRockets=0;
    rockets=malloc(numRockets*sizeof(Rocket));

    numParticles=0;
    particles=malloc(numParticles*sizeof(Particle)); 

    for(ui8 i=0; i<NUM_PLATFORMS; i++){
        platforms[i].x=rand()%(SCREENWIDTH-Images.platform.width-400)+200; //this is also used for random x when moving platform to the top
        platforms[i].y=SCREENHEIGHT-(i+1)*1000/NUM_PLATFORMS;
    } 

    pickup.x=-100;
    pickup.y=-100;
    pickup.id=1;    

    for(ui8 i=0; i<NUM_HEALTHPACKS; i++){
        healthPacks[i].x=-100;
        healthPacks[i].y=-100;
    }

    score=0;

    PlayMusicStream(music);
    PlayMusicStream(musicMenu);   

    //game loop
    while(!WindowShouldClose()){
        dt=GetFrameTime();
        mouse.x=GetMouseX();
        mouse.y=GetMouseY();

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
                            if(redSoldier.hp<=0){
                                gameState=2;
                                
                                //reset soundtrack
                                SeekMusicStream(music,0);
                                SeekMusicStream(musicMenu,0);
                            }
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
                if(redSoldier.pickupActive==1)
                    redSoldier.pickupActive=0;
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
                    SeekMusicStream(music,0);
                    SeekMusicStream(musicMenu,0);
                }
            }
            else{
                redSoldier.falling=1;
                redSoldier.speedY+=1000*dt;
            } 
        
            if((IsMouseButtonPressed(SHOOT) || IsKeyPressed(SHOOT_ALT)) && redSoldier.cooldown<0){
                redSoldier.cooldown=120;
                numRockets++;

                Rocket *buffer=malloc(sizeof(Rocket)*numRockets);

                Rocket newRocket={
                    .tx=LoadTextureFromImage(Images.rocket),
                    .x=redSoldier.x+MIDDLEX(redSoldier),
                    .y=redSoldier.y+MIDDLEY(redSoldier),
                    .rotation=90-atan2((redSoldier.x+MIDDLEX(redSoldier)-mouse.x),(redSoldier.y+MIDDLEY(redSoldier)-mouse.y))*180/PI,
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
            if(IsKeyPressed(USEPICKUP)){
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
        } 

        shift=redSoldier.speedY*dt;

        ClearBackground(BLACK); 
        BeginDrawing();

        //update background
        for(ui8 i=0; i<2; i++){
            //parallax scrolling
            if(bgY[i]>SCREENHEIGHT){
                bgY[i]=-SCREENHEIGHT;
                bgY[1-i]=0;
            } 
            if(redSoldier.y==SCREENMIDDLE(redSoldier) && redSoldier.speedY<0)
                bgY[i]-=shift/2;

            //draw background
            DrawTexture(backgrounds[i],0,bgY[i],WHITE);
        }

        //update platforms
        for(ui8 i=0; i<NUM_PLATFORMS; i++){
            //soldier collisions
            if(redSoldier.speedY>0)
                platformCollisionCheckS(&platforms[i],&redSoldier);

            if(redSoldier.y==SCREENMIDDLE(redSoldier) && redSoldier.speedY<0)
                platforms[i].y-=shift;

            //rocket collisions
            for(ui8 j=0; j<numRockets; j++)
                platformCollisionCheckR(&platforms[i],&rockets[j]);

            if(platforms[i].y>SCREENHEIGHT){
                platforms[i].x=rand()%(SCREENWIDTH-platforms[i].tx.width-400)+200;
                platforms[i].y=-platforms[i].tx.height;
                
                //random pickups and health packs
                ui8 random=rand()%10;
                if(random==0 && !VISIBLE(pickup)){ 
                    pickup.id=rand()%2+1;
                    pickup.tx=pickup.txs[pickup.id-1];
                    pickup.x=platforms[i].x+MIDDLEX(platforms[i])-MIDDLEX(pickup); 
                    pickup.y=platforms[i].y-pickup.tx.height; 
                }
                else if(random>7){
                    for(ui8 j=0; j<NUM_HEALTHPACKS; j++){
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
        for(ui8 i=0; i<NUM_HEALTHPACKS; i++){
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
   
        //text and hud
        switch(gameState){
            case 0: //game not started
                UpdateMusicStream(musicMenu);

                drawTextFullCenter("ROCKETMAN ADVENTURES",200, 100, WHITE);
                drawTextFullCenter(VERSION, 300,64, WHITE); 
                drawTextFullCenter("START JUMPING TO BEGIN",400,64, WHITE);
                break;
            case 1: //game in progress
                UpdateMusicStream(music);

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
   
    //unload images
    UnloadImage(Images.rocket);
    UnloadImage(Images.particleSmoke);
    
    //unload textures
    UnloadTexture(redSoldier.tx); 
    for(ui8 i=0; i<numRockets; i++)
        UnloadTexture(rockets[i].tx); 
    for(ui8 i=0; i<numParticles; i++)
        UnloadTexture(particles[i].tx);
    for(ui8 i=0; i<NUM_PLATFORMS; i++)
        UnloadTexture(platforms[i].tx);
    for(ui8 i=0; i<2; i++){
        UnloadTexture(backgrounds[i]);
        UnloadTexture(tryAgainButton.tx[i]);
    }
    for(ui8 i=0; i<NUM_HEALTHPACKS; i++){
        UnloadTexture(healthPacks[i].tx);
    }
    UnloadTexture(pickup.tx);
    UnloadTexture(parachute);
    UnloadTexture(healthHUD.tx);
    UnloadTexture(pickupHUD.tx);

    CloseAudioDevice();
    CloseWindow();

    return 0;
}
