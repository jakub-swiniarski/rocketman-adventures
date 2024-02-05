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
    float dt=1.0f;

    float volume=VOLUME;
    bool muted=MUTED;
    SetMasterVolume(volume);

    //load and resize images
    {
        Image image;

        LOAD_TEXTURE_ARRAY(red_soldier,6,5);
        LOAD_TEXTURE_ARRAY(pickup,NUM_PICKUP,8);
        LOAD_TEXTURE_ARRAY(button,2,8);
        LOAD_TEXTURE_ARRAY(bg,NUM_BG,5);

        LOAD_TEXTURE(red_soldier_jumping,5);
        LOAD_TEXTURE(rocket,3);
        LOAD_TEXTURE(launcher,5);
        LOAD_TEXTURE(particle_smoke,15);
        LOAD_TEXTURE(platform,5);
        LOAD_TEXTURE(parachute,5);
        LOAD_TEXTURE(hud,5);
        LOAD_TEXTURE(health_pack,6);

        UnloadImage(image);
    }

    //sfx, TODO: turn into an array and use enums to access elements
    Sound sfx_explosion=LoadSound(path_to_file("explosion.ogg"));
    Sound sfx_pickup=LoadSound(path_to_file("pickup.ogg"));
    Sound sfx_jump=LoadSound(path_to_file("jump.ogg"));
    Sound sfx_death=LoadSound(path_to_file("death.ogg"));

    //music
    Music music[NUM_MUSIC];
    for(int i=0; i<NUM_MUSIC; i++){
        char name[16];
        sprintf(name,"music%d.ogg",i);
        music[i]=LoadMusicStream(path_to_file(name));
    }

    int game_state;

    //player
    Soldier red_soldier={
        .tx=&TextureHolder.red_soldier[0], 
        .flip=1,
        .color=WHITE
    }; 

    //parachute
    Parachute parachute={
        .tx=&TextureHolder.parachute,
        .rotation=0
    };
        
    //rocket launcher
    Launcher rl={
        .tx=&TextureHolder.launcher,
        .x=0,
        .y=0,
        .rotation=0,
    };

    //background
    Background bg[2];
    int shift, level; //used for changing backgrounds

    srand(time(NULL));

    Rocket rockets={
        .next=NULL
    };

    Particle new_particle={
        .tx=&TextureHolder.particle_smoke,
        .x=-100,
        .y=-100,
        .rotation=0,
        .alpha=255,
        .is_free=1
    };
    Particle particles[MAX_PARTICLES]; //TODO: do the same as with the rockets

    Platform platforms[NUM_PLATFORMS];
    for(int i=0; i<NUM_PLATFORMS; i++)
        platforms[i].tx=&TextureHolder.platform;
    
    //pickups
    Pickup pickup={
        .tx=&TextureHolder.pickup[0], 
        .id=PARACHUTE
    };

    //health packs
    HealthPack new_health_pack={
        .tx=&TextureHolder.health_pack,
        .x=-100,
        .y=-100
    };
    HealthPack health_packs[NUM_HEALTH_PACKS];
    for(int i=0; i<NUM_HEALTH_PACKS; i++)
        health_packs[i]=new_health_pack;

    int score;
    char score_string[8];

    //hp hud
    HUD health_hud={
        .tx=&TextureHolder.hud,
        .x=5,
        .y=SCREEN_HEIGHT-TextureHolder.hud.height-5,
        .text="0"
    }; 
    sprintf(health_hud.text, "%d", red_soldier.hp);

    //pickup hud
    HUD pickup_hud={
        .tx=&TextureHolder.hud,
        .x=SCREEN_WIDTH-TextureHolder.hud.width-5,
        .y=SCREEN_HEIGHT-TextureHolder.hud.height-5,
        .text="EMPTY"
    };

    //buttons
    Button try_again_button={
        .tx=&TextureHolder.button[0],
        .x=SCREEN_WIDTH/2-TextureHolder.button[0].width/2,
        .y=500,
        .text="TRY AGAIN",
        .state=NORMAL
    };
    
    Vector2 mouse;
    
    START:
    game_state=MENU;
    level=1;

    red_soldier.x=(int)(SCREEN_WIDTH/2)-red_soldier.tx->width;
    red_soldier.y=SCREEN_HEIGHT-red_soldier.tx->height; 
    red_soldier.speed_x=red_soldier.speed_y=red_soldier.falling=0;
    red_soldier.rl_cooldown=red_soldier.anim_cooldown=0.0f;
    red_soldier.pickup=red_soldier.pickup_active=NONE;
    red_soldier.state=STANDING;
    red_soldier.slow_fall=red_soldier.crit_boost=1;
    red_soldier.color=WHITE;
    red_soldier.hp=200;

    for(int i=0; i<2; i++){
        bg[i].y=-i*SCREEN_HEIGHT;
        bg[i].tx=&TextureHolder.bg[i]; 
    }

    for(int i=0; i<MAX_PARTICLES; i++)
        particles[i]=new_particle;

    for(int i=0; i<NUM_PLATFORMS; i++){
        platforms[i].x=rand()%(SCREEN_WIDTH-TextureHolder.platform.width-400)+200; //this is also used for random x when moving platform to the top
        platforms[i].y=SCREEN_HEIGHT-(i+1)*1000/NUM_PLATFORMS;
    } 

    pickup.x=pickup.y=-100;
    pickup.id=PARACHUTE;    

    for(int i=0; i<NUM_HEALTH_PACKS; i++)
        health_packs[i]=new_health_pack;

    score=0;

    for(int i=0; i<NUM_MUSIC; i++)
        PlayMusicStream(music[i]);

    //game loop
    while(!WindowShouldClose()){
        dt=GetFrameTime();
        mouse=GetMousePosition();

        //volume control
        if(IsKeyPressed(VOL_UP) && volume<=0.9f){
            volume+=0.1f;
            SetMasterVolume(volume);
        }
        else if(IsKeyPressed(VOL_DOWN) && volume>=0.1f){
            volume-=0.1f;
            SetMasterVolume(volume);
        }
        else if(IsKeyPressed(MUTE)){
            muted=!muted;
            SetMasterVolume(muted?0:volume);
        }

        { //rocket explosions
            Rocket *r=&rockets;
            while(r->next!=NULL){
                rocket_border_check(r->next);

                if(r->next->collided){
                    if(r->next->should_explode){
                        PlaySound(sfx_explosion);

                        for(int j=0; j<MAX_PARTICLES; j++){
                            if(particles[j].is_free){
                                particles[j].x=r->next->x;
                                particles[j].y=r->next->y;
                                particles[j].rotation=rand()%360;
                                particles[j].is_free=0;
                                break;
                            }
                        }

                        Rocket rocket=*r->next;
                        if(abs(red_soldier.x+MIDDLE_X(red_soldier)-r->next->x-MIDDLE_X(rocket))<200
                        && abs(red_soldier.y+MIDDLE_Y(red_soldier)-r->next->y-MIDDLE_Y(rocket))<200
                        && game_state!=OVER){
                            //rocket jump
                            red_soldier.speed_x+=red_soldier.crit_boost*-1*r->next->speed_x;
                            red_soldier.speed_y+=red_soldier.crit_boost*-1*r->next->speed_y; 
                        
                            //damage
                            if(game_state==IN_PROGRESS){
                                red_soldier.hp-=20*red_soldier.crit_boost;
                                if(red_soldier.hp<=0)
                                    game_over(&game_state,&sfx_death,music);
                            }
                        }
                        
                        if(red_soldier.pickup_active==CRIT){
                            red_soldier.crit_boost=1;
                            red_soldier.color=WHITE;
                            red_soldier.pickup_active=NONE;
                        }
                    }
                    
                    //delete the rocket
                    Rocket *r_next=r->next->next;
                    free(r->next);
                    r->next=r_next;
                    break;
                }
                r=r->next;
            }
        }

        //prepare particles for future use
        for(int i=0; i<MAX_PARTICLES; i++)
            if(!particles[i].is_free && particles[i].alpha<5)
                particles[i]=new_particle;
 
        if(game_state!=OVER){
            //movement 
            if(IsKeyDown(MOVE_RIGHT) && !IsKeyDown(MOVE_LEFT)){
                red_soldier.x+=150*dt;
                red_soldier.state=WALKING;

                if(red_soldier.pickup_active==PARACHUTE && parachute.rotation>-30)
                    parachute.rotation-=60*dt;
            }
            else if(IsKeyDown(MOVE_LEFT) && !IsKeyDown(MOVE_RIGHT)){
                red_soldier.x-=150*dt;
                red_soldier.state=WALKING;
            
                if(red_soldier.pickup_active==PARACHUTE && parachute.rotation<30)
                    parachute.rotation+=60*dt;
            }
            else
                red_soldier.state=STANDING;
            if(red_soldier.speed_y<-100 || red_soldier.speed_y>100)
                red_soldier.state=JUMPING;
            //reset parachute rotation
            if(!IsKeyDown(MOVE_LEFT) && !IsKeyDown(MOVE_RIGHT)) //if not moving horizontally
                parachute.rotation+=parachute.rotation>0?-100*dt:100*dt;
            if(IsKeyDown(JUMP) && !red_soldier.falling){
                PlaySound(sfx_jump);
                if(red_soldier.pickup_active==PARACHUTE){
                    red_soldier.slow_fall=1;                     
                    red_soldier.pickup_active=NONE;
                }
                red_soldier.speed_y=-400;
            }

            //update rocket launcher
            rl.rotation=270-atan2((red_soldier.x+MIDDLE_X(red_soldier)-mouse.x),(red_soldier.y+MIDDLE_Y(red_soldier)-mouse.y))*180/PI; 
            if(mouse.x<red_soldier.x+MIDDLE_X(red_soldier)){
                red_soldier.flip=-1;
                rl.x=red_soldier.x+40;
            }
            else{
                rl.x=red_soldier.x+25;
                red_soldier.flip=1;
            }
            rl.y=red_soldier.y+45;

            
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
           
                if(game_state==MENU)
                    game_state=IN_PROGRESS;
            } 
        
            //gravity
            if(red_soldier.y+red_soldier.tx->height>=SCREEN_HEIGHT){
                if(game_state!=IN_PROGRESS){
                    red_soldier.y=SCREEN_HEIGHT-red_soldier.tx->height;
                    red_soldier.speed_y=0;
                    red_soldier.falling=0;
                }
                else
                    game_over(&game_state,&sfx_death,music);
            }
            else{
                red_soldier.falling=1;
                red_soldier.speed_y+=1000*dt;
            } 
        
            if((IsMouseButtonPressed(SHOOT) || IsKeyPressed(SHOOT_ALT)) && red_soldier.rl_cooldown<0.0f){
                red_soldier.rl_cooldown=0.8f;

                Rocket *r=&rockets;
                while(r->next!=NULL)
                    r=r->next;
                r->next=malloc(sizeof(Rocket));
                r=r->next;
                r->tx=&TextureHolder.rocket;
                r->x=red_soldier.x+MIDDLE_X(red_soldier);
                r->y=red_soldier.y+MIDDLE_Y(red_soldier)/4;
                r->rotation=90-atan2((red_soldier.x+MIDDLE_X(red_soldier)-mouse.x),(red_soldier.y+MIDDLE_Y(red_soldier)-mouse.y))*180/PI;
                r->speed_x=-960*cos(r->rotation*PI/180);
                r->speed_y=-960*sin(r->rotation*PI/180); 
                r->collided=0;
                r->should_explode=1;
                r->next=NULL;
            }

            //ACTIVATE PICKUP
            if(IsKeyPressed(USE_PICKUP)){
                red_soldier.pickup_active=red_soldier.pickup;
                red_soldier.pickup=NONE;
                switch(red_soldier.pickup_active){
                    case PARACHUTE:
                        red_soldier.slow_fall=0.2;
                    break;

                    case CRIT:
                        red_soldier.crit_boost=2;
                        red_soldier.color=RED;    
                    break; 
                }
            }
       
            //horizontal friction
            red_soldier.speed_x+=red_soldier.speed_x>0?-8:8;
            if(red_soldier.speed_x>-5 && red_soldier.speed_x<5)
                red_soldier.speed_x=0;

            soldier_border_check(&red_soldier);
    
            //update cooldowns
            red_soldier.rl_cooldown-=dt;  
        }  

        shift=red_soldier.speed_y*dt;

        ClearBackground(BLACK); 
        BeginDrawing();

        //update background
        for(int i=0; i<2; i++){
            //parallax scrolling
            if(bg[i].y>SCREEN_HEIGHT){
                bg[i].y=-SCREEN_HEIGHT;
                bg[1-i].y=0;

                level++;
                if(level>NUM_BG-1) level=7;
                bg[i].tx=&TextureHolder.bg[level];
            } 
            if(red_soldier.y==SCREEN_MIDDLE(red_soldier) && red_soldier.speed_y<0)
                bg[i].y-=shift/2;

            //draw background
            DrawTexture(*bg[i].tx,0,bg[i].y,WHITE);
        }

        //update platforms
        for(int i=0; i<NUM_PLATFORMS; i++){
            //soldier collisions
            if(red_soldier.speed_y>0)
                platform_collision_check_soldier(&platforms[i],&red_soldier);

            if(red_soldier.y==SCREEN_MIDDLE(red_soldier) && red_soldier.speed_y<0)
                platforms[i].y-=shift;

            { //rocket collisions
                Rocket *r=&rockets;
                while(r->next!=NULL){
                    r=r->next;
                    platform_collision_check_rocket(&platforms[i],r);
                }
            }

            if(platforms[i].y>SCREEN_HEIGHT){
                platforms[i].x=rand()%(SCREEN_WIDTH-platforms[i].tx->width-400)+200;
                platforms[i].y=-platforms[i].tx->height;
                
                //random pickups and health packs
                int random=rand()%10;
                if(random==0 && !IS_VISIBLE(pickup)){ 
                    pickup.id=rand()%NUM_PICKUP+1;
                    pickup.tx=&TextureHolder.pickup[pickup.id-1];
                    pickup.x=platforms[i].x+MIDDLE_X(platforms[i])-MIDDLE_X(pickup); 
                    pickup.y=platforms[i].y-pickup.tx->height; 
                }
                else if(random>7){
                    for(int j=0; j<NUM_HEALTH_PACKS; j++){
                        if(!IS_VISIBLE(health_packs[j])){
                            health_packs[j].x=platforms[i].x+MIDDLE_X(platforms[i])-MIDDLE_X(health_packs[j]);
                            health_packs[j].y=platforms[i].y-health_packs[j].tx->height;
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
                    health_packs[i]=new_health_pack;
                }
            }
            if(red_soldier.y==SCREEN_MIDDLE(red_soldier) && red_soldier.speed_y<0)
                health_packs[i].y-=shift;     
        }

        //parachute
        if(red_soldier.slow_fall<1){
            DrawTexturePro(
                *parachute.tx,
                (Rectangle){ //src
                    .x=0,
                    .y=0,
                    .width=parachute.tx->width,
                    .height=parachute.tx->height
                },
                (Rectangle){ //dest
                    .x=red_soldier.x+MIDDLE_X(red_soldier),
                    .y=red_soldier.y+10,
                    .width=parachute.tx->width,
                    .height=parachute.tx->height
                },
                (Vector2){ //origin
                    .x=(int)(parachute.tx->width/2),
                    .y=parachute.tx->height
                },
                parachute.rotation,
                WHITE
            ); 
        }
        
        //draw player
        switch(red_soldier.state){
            case STANDING:
                red_soldier.tx=&TextureHolder.red_soldier[0];
            break;

            case WALKING:
                red_soldier.anim_cooldown-=dt;
                if(red_soldier.anim_cooldown<0.0f){
                    red_soldier.frame++; 
                    red_soldier.tx=&TextureHolder.red_soldier[red_soldier.frame%6];
                    red_soldier.anim_cooldown=0.1;
                }
            break;

            case JUMPING:
                red_soldier.tx=&TextureHolder.red_soldier_jumping;
            break;
        }
        DRAW_PRO(red_soldier,red_soldier.flip,1,0,0,0,red_soldier.color);

        { //update rockets
            Rocket *r=&rockets;
            while(r->next!=NULL){
                r=r->next;
                Rocket rocket=*r;
                DRAW_PRO(rocket,1,1,rocket.rotation,MIDDLE_X(rocket),MIDDLE_Y(rocket),red_soldier.color)
                r->x+=r->speed_x*dt;
                r->y+=r->speed_y*dt;
            }
        }

        //draw rocket launcher
        DRAW_PRO(rl,1,red_soldier.flip,rl.rotation,50,45,red_soldier.color);

        //update particles
        for(int i=0; i<MAX_PARTICLES; i++){
            if(particles[i].is_free) continue;
            if(red_soldier.y==SCREEN_MIDDLE(red_soldier) && red_soldier.speed_y<0)
                particles[i].y-=shift;

            //fade away 
            particles[i].alpha-=2*dt;
            
            //draw
            DRAW_PRO(particles[i],1,1,particles[i].rotation,MIDDLE_X(particles[i]),MIDDLE_Y(particles[i]),WHITE);
        } 
   
        //text and hud
        switch(game_state){
            case MENU:
                UpdateMusicStream(music[0]);

                draw_text_full_center("ROCKETMAN ADVENTURES",200, 100, WHITE);
                draw_text_full_center(VERSION, 300,64, WHITE); 
                draw_text_full_center("START JUMPING TO BEGIN",400,64, WHITE);
                break;
            case IN_PROGRESS:
                if(level<7)
                    UpdateMusicStream(music[1]);
                else
                    UpdateMusicStream(music[2]);

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
                DRAW_PRO(pickup_hud,-1,1,0,0,0,WHITE);
                if(red_soldier.pickup!=NONE)
                    DrawTexture(TextureHolder.pickup[red_soldier.pickup-1],pickup_hud.x+150, pickup_hud.y+25, WHITE);
                else
                    draw_text_full(pickup_hud.text,pickup_hud.x+65,pickup_hud.y+40,64,WHITE);

                //score
                draw_text_full("SCORE:", 10, 10, 64, WHITE);
                draw_text_full(score_string,250, 10, 64, WHITE);
                break;
            case OVER:
                //update buttons
                if(MOUSE_HOVER_BUTTON(try_again_button,mouse)){
                    try_again_button.state=HOVER;
                    if(IsMouseButtonPressed(SHOOT))
                        goto START;
                }
                else
                    try_again_button.state=NORMAL;

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
    for(int i=0; i<6; i++)
        UnloadTexture(TextureHolder.red_soldier[i]);
    UnloadTexture(TextureHolder.rocket);
    UnloadTexture(TextureHolder.launcher);
    UnloadTexture(TextureHolder.parachute);
    UnloadTexture(TextureHolder.platform);
    for(int i=0; i<NUM_PICKUP; i++)
        UnloadTexture(TextureHolder.pickup[i]);
    UnloadTexture(TextureHolder.health_pack);
    UnloadTexture(TextureHolder.hud);
    for(int i=0; i<2; i++)
        UnloadTexture(TextureHolder.button[i]);
    UnloadTexture(TextureHolder.particle_smoke);
    for(int i=0; i<NUM_BG; i++)
        UnloadTexture(TextureHolder.bg[i]);

    //unload sfx
    UnloadSound(sfx_explosion);
    UnloadSound(sfx_pickup);
    UnloadSound(sfx_jump);
    UnloadSound(sfx_death);

    //unload music
    for(int i=0; i<NUM_MUSIC; i++)
        UnloadMusicStream(music[i]);

    CloseAudioDevice();
    CloseWindow();

    return 0;
}
