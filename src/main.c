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
#include "macros.h"
#include "config.h"

int main(void){
    InitWindow(SCREEN_WIDTH,SCREEN_HEIGHT,"Rocketman Adventures");
    
    int display=GetCurrentMonitor();
    SetWindowSize(GetMonitorWidth(display),GetMonitorHeight(display));
    ToggleFullscreen();

    InitAudioDevice();

    SetTargetFPS(FPS);
    float dt=1.f;

    SetMasterVolume((float)volume/100);

    //load and resize images
    Images.red_soldier=LoadImage(path_to_file("red_soldier.png"));
    ImageResizeNN(&Images.red_soldier,12*5,20*5);

    Images.rocket=LoadImage(path_to_file("rocket.png"));
    ImageResizeNN(&Images.rocket,30*3,8*3);

    Images.launcher=LoadImage(path_to_file("launcher.png"));
    ImageResizeNN(&Images.launcher,20*5,8*5);

    Images.particle_smoke=LoadImage(path_to_file("particle_smoke.png"));
    ImageResizeNN(&Images.particle_smoke,12*12,12*12);

    Images.platform=LoadImage(path_to_file("platform.png"));
    ImageResizeNN(&Images.platform,30*5,2*5);

    Images.parachute_pickup=LoadImage(path_to_file("parachute_pickup.png"));
    ImageResizeNN(&Images.parachute_pickup, 16*4, 20*4);

    Images.parachute=LoadImage(path_to_file("parachute.png"));
    ImageResizeNN(&Images.parachute, 13*8, 11*8);

    Images.crit_pickup=LoadImage(path_to_file("crit_pickup.png"));
    ImageResizeNN(&Images.crit_pickup,9*8,13*8);

    Images.hud=LoadImage(path_to_file("hud.png"));
    ImageResizeNN(&Images.hud,64*5,32*5);

    Images.health_pack=LoadImage(path_to_file("health_pack.png"));
    ImageResizeNN(&Images.health_pack,12*6,12*6);

    Images.button[0]=LoadImage(path_to_file("button_normal.png"));
    ImageResizeNN(&Images.button[0], 50*8, 20*8);

    Images.button[1]=LoadImage(path_to_file("button_hover.png"));
    ImageResizeNN(&Images.button[1], 50*8, 20*8);

    //backgrounds 
    Texture bg_txs[NUM_BG];
    for(int i=0; i<NUM_BG; i++){
        char name[12]="bg";
        char num[2];
        sprintf(num,"%d",i);
        strcat(name,num);
        strcat(name,".png");
        Images.bg[i]=LoadImage(path_to_file(name));
        ImageResizeNN(&Images.bg[i],SCREEN_WIDTH,SCREEN_HEIGHT);
        bg_txs[i]=LoadTextureFromImage(Images.bg[i]); 
        UnloadImage(Images.bg[i]);
    }
        
    //sfx
    Sound sfx_explosion=LoadSound(path_to_file("explosion.ogg"));
    Sound sfx_pickup=LoadSound(path_to_file("pickup.ogg"));
    Sound sfx_jump=LoadSound(path_to_file("jump.ogg"));
    Sound sfx_death=LoadSound(path_to_file("death.ogg"));

    //music
    Music music_menu=LoadMusicStream(path_to_file("soundtrack_menu.ogg"));
    Music music_normal=LoadMusicStream(path_to_file("soundtrack_normal.ogg"));
    Music music_space=LoadMusicStream(path_to_file("soundtrack_space.ogg"));

    int game_state; //0 - not started, 1 - in progress, 2 - game over

    //player
    Soldier red_soldier={
        .tx=LoadTextureFromImage(Images.red_soldier), 
    }; 
    UnloadImage(Images.red_soldier);  

    //parachute
    Texture parachute=LoadTextureFromImage(Images.parachute);
    UnloadImage(Images.parachute);
    int rotation_parachute=0; 

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
    Texture bgs[2]; //TODO: TURN THESE TWO (115,116) into a struct
    int bg_y[2];  
    int shift, level; //used for changing backgrounds

    srand(time(NULL));

    Rocket new_rocket={
        .tx=LoadTextureFromImage(Images.rocket),
        .x=-100,
        .y=-100,
        .speed_x=0,
        .speed_y=0,
        .rotation=0,
        .collided=0,
        .should_explode=1,
        .is_free=1
    };
    Rocket rockets[MAX_ROCKETS]; 
    UnloadImage(Images.rocket);       

    Particle new_particle={
        .tx=LoadTextureFromImage(Images.particle_smoke),
        .x=-100,
        .y=-100,
        .rotation=0,
        .alpha=255,
        .is_free=1
    };
    Particle particles[MAX_PARTICLES];
    UnloadImage(Images.particle_smoke);

    Platform platforms[NUM_PLATFORMS];
    for(int i=0; i<NUM_PLATFORMS; i++){
        Platform new_platform={
            .tx=LoadTextureFromImage(Images.platform),
        };
        platforms[i]=new_platform;
    }
    UnloadImage(Images.platform);
    
    //pickups
    Pickup pickup={
        .txs[0]=LoadTextureFromImage(Images.parachute_pickup),
        .txs[1]=LoadTextureFromImage(Images.crit_pickup),
        .tx=LoadTextureFromImage(Images.parachute_pickup), 
    };
    UnloadImage(Images.parachute_pickup);
    UnloadImage(Images.crit_pickup);

    //health packs
    Health_pack health_packs[NUM_HEALTH_PACKS];
    for(int i=0; i<NUM_HEALTH_PACKS; i++){
        Health_pack new_health_pack={ //TODO: SIMPLIFY THIS, create a new healthpack once with x=-100 and y=-100 and assign healtpacks[i] to new health pack when it gets picked, when it disappears and when the game is initialized
            .tx=LoadTextureFromImage(Images.health_pack),
        };
        health_packs[i]=new_health_pack;
    }
    UnloadImage(Images.health_pack);

    int score;
    char score_string[5];

    //hp hud
    HUD health_hud={
        .tx=LoadTextureFromImage(Images.hud),
        .x=5,
        .y=SCREEN_HEIGHT-Images.hud.height-5,
        .text="0"
    }; 
    sprintf(health_hud.text, "%d", red_soldier.hp);

    //pickup hud
    HUD pickup_hud={
        .tx=LoadTextureFromImage(Images.hud),
        .x=SCREEN_WIDTH-Images.hud.width-5,
        .y=SCREEN_HEIGHT-Images.hud.height-5,
        .text="EMPTY"
    };
    UnloadImage(Images.hud); 

    //buttons
    Button try_again_button={
        .tx[0]=LoadTextureFromImage(Images.button[0]),
        .tx[1]=LoadTextureFromImage(Images.button[1]),
        .x=SCREEN_WIDTH/2-Images.button[0].width/2,
        .y=500,
        .text="TRY AGAIN"
    };
    for(int i=0; i<2; i++)
        UnloadImage(Images.button[i]);
    
    Vector2 mouse;
    
    START:
    game_state=0;
    level=1;

    red_soldier.x=(int)(SCREEN_WIDTH/2)-red_soldier.tx.width;
    red_soldier.y=SCREEN_HEIGHT-red_soldier.tx.height; 
    red_soldier.speed_x=red_soldier.speed_y=red_soldier.cooldown=red_soldier.falling=0;
    red_soldier.slow_fall=red_soldier.crit_boost=1;
    red_soldier.hp=200;

    for(int i=0; i<2; i++){
        bg_y[i]=-i*SCREEN_HEIGHT;
        bgs[i]=bg_txs[i]; 
    }

    for(int i=0; i<MAX_ROCKETS; i++)
        rockets[i]=new_rocket;  

    for(int i=0; i<MAX_PARTICLES; i++)
        particles[i]=new_particle;

    for(int i=0; i<NUM_PLATFORMS; i++){
        platforms[i].x=rand()%(SCREEN_WIDTH-Images.platform.width-400)+200; //this is also used for random x when moving platform to the top
        platforms[i].y=SCREEN_HEIGHT-(i+1)*1000/NUM_PLATFORMS;
    } 

    pickup.x=pickup.y=-100;
    pickup.id=1;    

    for(int i=0; i<NUM_HEALTH_PACKS; i++)
        health_packs[i].x=health_packs[i].y=-100;

    score=0;

    PlayMusicStream(music_normal);
    PlayMusicStream(music_menu);   
    PlayMusicStream(music_space);

    //game loop
    while(!WindowShouldClose()){
        dt=GetFrameTime();
        mouse=GetMousePosition();

        //volume control
        if(IsKeyPressed(VOL_UP) && volume<=95){
            volume+=10;
            SetMasterVolume((float)volume/100);
        }
        else if(IsKeyPressed(VOL_DOWN) && volume>=5){
            volume-=10;
            SetMasterVolume((float)volume/100);
        }
        else if(IsKeyPressed(MUTE)){
            muted=!muted;
            SetMasterVolume(muted?0:(float)volume/100);
        }

        for(int i=0; i<MAX_ROCKETS; i++){
            if(rockets[i].is_free) continue;
            rocket_border_check(&rockets[i]);

            if(rockets[i].collided){
                PlaySound(sfx_explosion);

                //smoke particles
                if(rockets[i].should_explode){
                    for(int j=0; j<MAX_PARTICLES; j++){
                        if(particles[j].is_free){
                            particles[j].x=rockets[i].x;
                            particles[j].y=rockets[i].y;
                            particles[j].rotation=rand()%360;
                            particles[j].is_free=0;
                            break;
                        }
                    }

                    if(abs(red_soldier.x+MIDDLE_X(red_soldier)-rockets[i].x-MIDDLE_X(rockets[i]))<100 
                    && abs(red_soldier.y+MIDDLE_Y(red_soldier)-rockets[i].y-MIDDLE_Y(rockets[i]))<100
                    && game_state!=2){
                        //rocket jump
                        red_soldier.speed_x=red_soldier.crit_boost*-1*rockets[i].speed_x;
                        red_soldier.speed_y=red_soldier.crit_boost*-1*rockets[i].speed_y; 
                    
                        //damage
                        if(game_state==1){
                            red_soldier.hp-=20*red_soldier.crit_boost;
                            if(red_soldier.hp<=0){
                                game_state=2;
                                PlaySound(sfx_death);

                                //reset soundtrack - same thing happens when player hits the ground
                                SeekMusicStream(music_normal,0);
                                SeekMusicStream(music_menu,0);
                                SeekMusicStream(music_space,0);
                            } 
                        }
                    }
                    
                    if(red_soldier.pickup_active==2){
                        red_soldier.crit_boost=1;
                        rl.color=WHITE;
                        red_soldier.pickup_active=0;
                    }
                }
                
                //prepare for shooting again
                rockets[i]=new_rocket; 
            }
        }

        //prepare particles for future use
        for(int i=0; i<MAX_PARTICLES; i++)
            if(!particles[i].is_free && particles[i].alpha<5)
                particles[i]=new_particle;
 
        if(game_state!=2){
            //movement 
            if(IsKeyDown(MOVE_RIGHT) && !IsKeyDown(MOVE_LEFT)){
                red_soldier.x+=150*dt;

                if(red_soldier.pickup_active==1 && rotation_parachute>-30)
                    rotation_parachute-=60*dt;
            }
            if(IsKeyDown(MOVE_LEFT) && !IsKeyDown(MOVE_RIGHT)){
                red_soldier.x-=150*dt;
            
                if(red_soldier.pickup_active==1 && rotation_parachute<30)
                    rotation_parachute+=60*dt;
            }
            //reset parachute rotation
            if(!IsKeyDown(MOVE_LEFT) && !IsKeyDown(MOVE_RIGHT)) //if not moving horizontally
                rotation_parachute+=rotation_parachute>0?-100*dt:100*dt;
            if(IsKeyDown(JUMP) && !red_soldier.falling){
                PlaySound(sfx_jump);
                if(red_soldier.pickup_active==1){
                    red_soldier.slow_fall=1;                     
                    red_soldier.pickup_active=0;
                }
                red_soldier.speed_y=-400;
            }

            //update rocket launcher
            rl.x=red_soldier.x+MIDDLE_X(red_soldier);
            rl.y=red_soldier.y+MIDDLE_Y(red_soldier); 
            rl.rotation=270-atan2((red_soldier.x+MIDDLE_X(red_soldier)-mouse.x),(red_soldier.y+MIDDLE_Y(red_soldier)-mouse.y))*180/PI; 
        
            //flip the rocket launcher to prevent it from going upside down
            rl.flip=mouse.x<red_soldier.x+MIDDLE_X(red_soldier)?-1:1;
            
            //update player position
            red_soldier.x+=red_soldier.speed_x*dt;
            if(red_soldier.speed_y>0 && red_soldier.falling)
                red_soldier.y+=red_soldier.speed_y*dt*red_soldier.slow_fall; 
            else
                red_soldier.y+=red_soldier.speed_y*dt; 

            if(red_soldier.y<SCREEN_MIDDLE(red_soldier)){
                //score
                score-=red_soldier.speed_y*dt;
                sprintf(score_string, "%d", score);

                red_soldier.y=SCREEN_MIDDLE(red_soldier); 
           
                if(game_state==0)
                    game_state=1;
            } 
        
            //gravity
            if(red_soldier.y+red_soldier.tx.height>=SCREEN_HEIGHT){
                if(game_state!=1){
                    red_soldier.y=SCREEN_HEIGHT-red_soldier.tx.height;
                    red_soldier.speed_y=0;
                    red_soldier.falling=0;
                }
                else{
                    game_state=2;
                    PlaySound(sfx_death);

                    //reset soundtrack
                    SeekMusicStream(music_normal,0);
                    SeekMusicStream(music_menu,0);
                    SeekMusicStream(music_space,0);
                }
            }
            else{
                red_soldier.falling=1;
                red_soldier.speed_y+=1000*dt;
            } 
        
            if((IsMouseButtonPressed(SHOOT) || IsKeyPressed(SHOOT_ALT)) && red_soldier.cooldown<0){
                red_soldier.cooldown=120;

                for(int i=0; i<MAX_ROCKETS; i++){
                    if(rockets[i].is_free){
                        rockets[i].is_free=0;
                        rockets[i].x=red_soldier.x+MIDDLE_X(rockets[i]);
                        rockets[i].y=red_soldier.y+MIDDLE_Y(rockets[i]);
                        rockets[i].rotation=90-atan2((red_soldier.x+MIDDLE_X(red_soldier)-mouse.x),(red_soldier.y+MIDDLE_Y(red_soldier)-mouse.y))*180/PI;
                        rockets[i].speed_x=-1.2*cos(rockets[i].rotation*PI/180)*800;
                        rockets[i].speed_y=-1.2*sin(rockets[i].rotation*PI/180)*800; 
                        break;
                    }
                }   
            }

            //ACTIVATE PICKUP
            if(IsKeyPressed(USE_PICKUP)){
                red_soldier.pickup_active=red_soldier.pickup;
                red_soldier.pickup=0;
                switch(red_soldier.pickup_active){
                    case 1:
                        red_soldier.slow_fall=0.2;
                    break;

                    case 2:
                        red_soldier.crit_boost=2;
                        rl.color=RED;    
                    break; 
                }
            }
       
            //horizontal friction
            red_soldier.speed_x+=red_soldier.speed_x>0?-8:8;
            if(red_soldier.speed_x>-5 && red_soldier.speed_x<5)
                red_soldier.speed_x=0;

            soldier_border_check(&red_soldier);
    
            //update cooldowns
            red_soldier.cooldown-=150*dt;

            //update rockets
            for(int i=0; i<MAX_ROCKETS; i++){
                //position
                rockets[i].x+=rockets[i].speed_x*dt;
                rockets[i].y+=rockets[i].speed_y*dt;
            }  
        }  

        shift=red_soldier.speed_y*dt;

        ClearBackground(BLACK); 
        BeginDrawing();

        //update background
        for(int i=0; i<2; i++){
            //parallax scrolling
            if(bg_y[i]>SCREEN_HEIGHT){
                bg_y[i]=-SCREEN_HEIGHT;
                bg_y[1-i]=0;

                level++;
                if(level>NUM_BG-1) level=NUM_BG-1;
                bgs[i]=bg_txs[level];
            } 
            if(red_soldier.y==SCREEN_MIDDLE(red_soldier) && red_soldier.speed_y<0)
                bg_y[i]-=shift/2;

            //draw background
            DrawTexture(bgs[i],0,bg_y[i],WHITE);
        }

        //update platforms
        for(int i=0; i<NUM_PLATFORMS; i++){
            //soldier collisions
            if(red_soldier.speed_y>0)
                platform_collision_check_soldier(&platforms[i],&red_soldier);

            if(red_soldier.y==SCREEN_MIDDLE(red_soldier) && red_soldier.speed_y<0)
                platforms[i].y-=shift;

            //rocket collisions
            for(int j=0; j<MAX_ROCKETS; j++)
                platform_collision_check_rocket(&platforms[i],&rockets[j]);

            if(platforms[i].y>SCREEN_HEIGHT){
                platforms[i].x=rand()%(SCREEN_WIDTH-platforms[i].tx.width-400)+200;
                platforms[i].y=-platforms[i].tx.height;
                
                //random pickups and health packs
                int random=rand()%10;
                if(random==0 && !IS_VISIBLE(pickup)){ 
                    pickup.id=rand()%2+1;
                    pickup.tx=pickup.txs[pickup.id-1];
                    pickup.x=platforms[i].x+MIDDLE_X(platforms[i])-MIDDLE_X(pickup); 
                    pickup.y=platforms[i].y-pickup.tx.height; 
                }
                else if(random>7){
                    for(int j=0; j<NUM_HEALTH_PACKS; j++){
                        if(!IS_VISIBLE(health_packs[j])){
                            health_packs[j].x=platforms[i].x+MIDDLE_X(platforms[i])-MIDDLE_X(health_packs[j]);
                            health_packs[j].y=platforms[i].y-health_packs[j].tx.height;
                            break;
                        }
                    }
                }
            }

            //draw platforms
            DRAW(platforms[i]);
        }

        //update pickup
        if(pickup_collect_check(&pickup, &red_soldier)) 
            PlaySound(sfx_pickup); 
        if(red_soldier.y==SCREEN_MIDDLE(red_soldier) && red_soldier.speed_y<0)
            pickup.y-=shift; 
        if(IS_VISIBLE(pickup))
            DRAW(pickup);

        //update health packs
        for(int i=0; i<NUM_HEALTH_PACKS; i++){
            if(IS_VISIBLE(health_packs[i])){
                DRAW(health_packs[i]);
                if(COLLISION(health_packs[i],red_soldier)){
                    red_soldier.hp+=50;
                    health_packs[i].x=health_packs[i].y=-100;
                }
            }
            if(red_soldier.y==SCREEN_MIDDLE(red_soldier) && red_soldier.speed_y<0)
                health_packs[i].y-=shift;     
        }

        //parachute
        if(red_soldier.slow_fall<1){
            DrawTexturePro(
                parachute,
                (Rectangle){ //src
                    .x=0,
                    .y=0,
                    .width=parachute.width,
                    .height=parachute.height
                },
                (Rectangle){ //dest
                    .x=red_soldier.x+MIDDLE_X(red_soldier),
                    .y=red_soldier.y,
                    .width=parachute.width,
                    .height=parachute.height
                },
                (Vector2){ //origin
                    .x=(int)(parachute.width/2),
                    .y=parachute.height
                },
                rotation_parachute,
                WHITE
            ); 
        }
        
        //draw player
        DRAW(red_soldier);

        //draw rockets
        for(int i=0; i<MAX_ROCKETS; i++){
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
                    .x=MIDDLE_X(rockets[i]),
                    .y=MIDDLE_Y(rockets[i])
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
                .x=MIDDLE_X(rl),
                .y=MIDDLE_Y(rl)
            },
            rl.rotation,
            rl.color
        ); 

        //update particles
        for(int i=0; i<MAX_PARTICLES; i++){
            if(particles[i].is_free) continue;
            if(red_soldier.y==SCREEN_MIDDLE(red_soldier) && red_soldier.speed_y<0)
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
                    .x=MIDDLE_X(particles[i]),
                    .y=MIDDLE_Y(particles[i])
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
        switch(game_state){
            case 0: //game not started
                UpdateMusicStream(music_menu);

                draw_text_full_center("ROCKETMAN ADVENTURES",200, 100, WHITE);
                draw_text_full_center(VERSION, 300,64, WHITE); 
                draw_text_full_center("START JUMPING TO BEGIN",400,64, WHITE);
                break;
            case 1: //game in progress
                if(level<7)
                    UpdateMusicStream(music_normal);
                else
                    UpdateMusicStream(music_space);

                if(red_soldier.hp<50)
                    health_hud.text_color=TEXT_COLOR[0];
                else if(red_soldier.hp>200)
                    health_hud.text_color=TEXT_COLOR[2];
                else
                    health_hud.text_color=TEXT_COLOR[1];

                //hp hud
                sprintf(health_hud.text,"%d",red_soldier.hp);
                DRAW(health_hud);
                draw_text_full(health_hud.text,health_hud.x+40,health_hud.y+30,100, health_hud.text_color); 
               
                //pickup hud
                DrawTexturePro(
                    pickup_hud.tx,
                    (Rectangle){ //src
                        .x=0,
                        .y=0,
                        .width=-1*pickup_hud.tx.width,
                        .height=pickup_hud.tx.height
                    },
                    (Rectangle){ //dest
                        .x=pickup_hud.x,
                        .y=pickup_hud.y,
                        .width=pickup_hud.tx.width,
                        .height=pickup_hud.tx.height
                    },
                    (Vector2){ //origin
                        .x=0,
                        .y=0
                    },
                    0,
                    WHITE
                );
                if(red_soldier.pickup==1 || red_soldier.pickup==2)
                    DrawTexture(pickup.txs[red_soldier.pickup-1],pickup_hud.x+150, pickup_hud.y+25, WHITE);
                else
                    draw_text_full(pickup_hud.text,pickup_hud.x+65,pickup_hud.y+40,64,WHITE);

                //score
                draw_text_full("SCORE:", 10, 10, 64, WHITE);
                draw_text_full(score_string,250, 10, 64, WHITE);
                break;
            case 2: //game over
                //update buttons
                if(MOUSE_HOVER_BUTTON(try_again_button,mouse)){
                    try_again_button.state=1;
                    if(IsMouseButtonPressed(SHOOT))
                        goto START;
                }
                else
                    try_again_button.state=0;

                DrawRectangle(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,(Color){0,0,0,150});
                draw_text_full_center("GAME OVER",200,100, WHITE);
                draw_text_full_center("SCORE:",300,64, WHITE);
                draw_text_full_center(score_string,375,64, WHITE);
                DrawTexture(try_again_button.tx[try_again_button.state],try_again_button.x,try_again_button.y,WHITE);
                draw_text_full(try_again_button.text, try_again_button.x+12, try_again_button.y+45, 64, WHITE);
                break;
            default:
                draw_text_full("ERROR", 100, 100, 120, WHITE);
        } 

        EndDrawing();
    }
   
    //unload textures
    UnloadTexture(red_soldier.tx); 
    for(int i=0; i<MAX_ROCKETS; i++)
        UnloadTexture(rockets[i].tx); 
    for(int i=0; i<MAX_PARTICLES; i++)
        UnloadTexture(particles[i].tx);
    for(int i=0; i<NUM_PLATFORMS; i++)
        UnloadTexture(platforms[i].tx);
    for(int i=0; i<NUM_BG; i++)
        UnloadTexture(bg_txs[i]);
    for(int i=0; i<2; i++){
        UnloadTexture(bgs[i]);
        UnloadTexture(try_again_button.tx[i]);
    }
    for(int i=0; i<NUM_HEALTH_PACKS; i++){
        UnloadTexture(health_packs[i].tx);
    }
    UnloadTexture(pickup.tx);
    UnloadTexture(parachute);
    UnloadTexture(health_hud.tx);
    UnloadTexture(pickup_hud.tx);

    //unload sfx
    UnloadSound(sfx_explosion);
    UnloadSound(sfx_pickup);
    UnloadSound(sfx_jump);
    UnloadSound(sfx_death);

    //unload music
    UnloadMusicStream(music_menu);
    UnloadMusicStream(music_normal);
    UnloadMusicStream(music_space);

    CloseAudioDevice();
    CloseWindow();

    return 0;
}
