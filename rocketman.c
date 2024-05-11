#include <raylib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

#include "config.h"

/* macros */
#define COLLISION(X, Y) ((X.x + X.tx->width > Y.x && X.x < Y.x + Y.tx->width) && (X.y + X.tx->height > Y.y && X.y < Y.y + Y.tx->height))
#define DRAW(X) DrawTexture(*X.tx, X.x, X.y, WHITE)
#define DRAW_PRO(X, FH, FV, R, OX, OY, C) (DrawTexturePro(*X.tx, (Rectangle){ .x = 0, .y = 0, .width = X.tx->width * FH, .height = X.tx->height * FV }, (Rectangle){ .x = X.x, .y = X.y, .width = X.tx->width, .height = X.tx->height }, (Vector2){ .x = OX, .y = OY }, R, C)) /* X, flip horizontal, flip vertical, rotation, origin x, origin y, color */
#define IS_VISIBLE(X) ((X.x + X.tx->width > 0 && X.x < screen_width) && (X.y + X.tx->height > 0 && X.y < screen_width))
#define LOAD_TEXTURE(X, S) {\
    image = LoadImage(path_to_file(#X ".png"));\
    ImageResizeNN(&image, image.width * S, image.height * S);\
    texture_holder.X = LoadTextureFromImage(image);\
}
#define LOAD_TEXTURE_ARRAY(X, N, S) {\
    for (int i = 0; i < N; i++) {\
        char name[20];\
        sprintf(name, #X "%d.png", i);\
        image = LoadImage(path_to_file(name));\
        ImageResizeNN(&image, image.width * S, image.height * S);\
        texture_holder.X[i] = LoadTextureFromImage(image);\
    }\
}
#define MIDDLE_X(X) ((int)(X.tx->width / 2))
#define MIDDLE_Y(X) ((int)(X.tx->height / 2))
#define MOUSE_HOVER_BUTTON(X, Y) ((Y.x > X.x && Y.x < X.x + X.tx->width) && (Y.y > X.y && Y.y < X.y + X.tx->height))
#define SCREEN_MIDDLE(X) ((int)(screen_height / 2) - (int)(X.tx->height / 2))

#define NUM_BG 11
#define NUM_MUSIC 3
#define RANDOM_PLATFORM_X (rand() % (screen_width - texture_holder.platform.width - 400) + 200)

/* enums */
enum { state_standing, state_walking, state_jumping };
enum { game_menu, game_active, game_over };
enum { pickup_none, pickup_parachute, pickup_crit, num_pickup };
enum { button_normal, button_hover };
enum { sfx_explosion, sfx_pickup, sfx_jump, sfx_death, num_sfx };
enum { music_menu, music_normal, music_space};
enum { col_low, col_normal, col_high };

/* structs */
typedef struct {
    Texture *tx;
    int y;
} Background;

typedef struct {
    Texture *tx;
    int x, y;
    char text[12];
    int state;
} Button;

typedef struct {
    Texture *tx;
    int x, y;
} HealthPack;

typedef struct {
    Texture *tx;
    int x, y;
    char text[16];
    Color text_color;
} HUD;

typedef struct {
    Texture *tx;
    int x, y;
    int rotation;
} Launcher;

typedef struct {
    Texture *tx;
    int rotation;
} Parachute;

typedef struct Particle Particle;
struct Particle {
    Texture *tx;
    int x, y;
    int rotation;
    int alpha;
    Particle *next;
};

typedef struct {
    Texture *tx;
    int x, y;
    int id;
} Pickup;

typedef struct {
    Texture *tx;
    int x, y;
} Platform;

typedef struct Rocket Rocket;
struct Rocket {
    Texture *tx;
    int x, y;
    int rotation;
    int speed_x, speed_y;
    bool collided, should_explode;
    Rocket *next;
};

typedef struct {
    Texture *tx;
    int x, y;
    int speed_x, speed_y;
    float rl_cooldown;
    bool falling;
    int pickup, pickup_active;
    float gravity_factor;
    float rl_knockback_factor;
    int hp;
    int flip;
    Color color;
    int state;
    float anim_cooldown;
    int frame;
} Soldier;

typedef struct {
    Texture red_soldier[6];
    Texture red_soldier_jumping;
    Texture rocket;
    Texture launcher;
    Texture parachute; 
    Texture platform;
    Texture pickup[num_pickup];
    Texture healthpack;
    Texture hud; 
    Texture button[2];
    Texture particle_smoke;
    Texture bg[NUM_BG];
} TextureHolder;

/* function declarations */
static void close(void);
static void draw_text(const char *text, int x, int y, int font_size, Color color);
static void draw_text_center(const char *text, int y, int font_size, Color color);
static void end_game(int *gs, Sound *sfx, Music *m);
static void gravity(void);
static void init(void);
static void input(void);
static void load_assets(void);
static void manage_rockets(void);
static char *path_to_file(const char *name);
static bool pickup_collect_check(Pickup *p, Soldier *r);
static void platform_collision_check_rocket(Platform *p, Rocket *r);
static void platform_collision_check_soldier(Platform *p, Soldier *s);
static void restart(void);
static void rocket_border_check(Rocket *r);
static void run(void);
static void soldier_border_check(Soldier *s);
static void spawn_healthpack(int x, int y);
static void spawn_particle(Rocket *r);
static void spawn_pickup(int x, int y);
static void spawn_rocket(void);
static void unload_assets(void);
static void update_bg(void);
static void update_healthpacks(void);
static void update_hud(void);
static void update_music(void);
static void update_parachute(void);
static void update_particles(void);
static void update_pickup(void);
static void update_platforms(void);
static void update_rl(void);
static void update_rockets(void);
static void update_score(void);
static void update_soldier(void);
static void volume_control(void);

/* variables */
static Background bg[2];
static int display;
static float dt;
static int game_state;
static HUD health_hud;
static HealthPack healthpacks[NUM_HEALTHPACKS];
static int level;
static Vector2 mouse;
static bool movement_allowed;
static Music music[NUM_MUSIC];
static HealthPack new_healthpack;
static Parachute parachute;
static Particle particles;
static Pickup pickup;
static HUD pickup_hud;
static Platform platforms[NUM_PLATFORMS];
static int score;
static char score_string[8];
static bool should_shift;
static Soldier red_soldier;
static Launcher rl;
static Rocket rockets;
static int shift;
static Sound sfx[num_sfx];
static TextureHolder texture_holder;
static Button try_again_button;

/* constants */
static const char *directory = "res/";
static const char *version = "3.1.4";

/* function implementations */
void close(void) {
    CloseAudioDevice();
    CloseWindow();
}

void draw_text(const char *text, int x, int y, int font_size, Color color) {
    DrawText(text, x, y, font_size, BLACK);
    DrawText(text, x + 7, y + 7, font_size, color);
}

void draw_text_center(const char *text, int y, int font_size, Color color) {
    draw_text(
        text,
        (int)(screen_width / 2) - (int)(MeasureTextEx(GetFontDefault(), text, font_size, 10).x / 2),
        y,
        font_size,
        color
    );
}

void end_game(int *gs, Sound *sfx, Music *m) {
    *gs = game_over;
    PlaySound(*sfx);
    for (int i = 0; i < NUM_MUSIC; i++)
        SeekMusicStream(m[i], 0.0f);
}

void gravity(void) {
    if (!movement_allowed)
        return;

    if (red_soldier.y + red_soldier.tx->height >= screen_height) {
        if (game_state == game_menu) {
            red_soldier.y = screen_height - red_soldier.tx->height;
            red_soldier.speed_y = 0;
            red_soldier.falling = 0;
        } else
            end_game(&game_state, &sfx[sfx_death], music);
    } else {
        red_soldier.falling = 1;
        red_soldier.speed_y += accel_gravity * dt;
    } 
}

void init(void) {
    InitWindow(screen_width, screen_height, "Rocketman Adventures");

    display = GetCurrentMonitor();
    SetWindowSize(GetMonitorWidth(display), GetMonitorHeight(display));
    ToggleFullscreen();

    InitAudioDevice();

    SetTargetFPS(fps);
    SetMasterVolume(volume);
    
    srand(time(NULL));

    load_assets();

    dt = 1.0f;

    red_soldier.tx = &texture_holder.red_soldier[0];
    red_soldier.flip = 1;
    red_soldier.rl_cooldown = 0.0f;
    red_soldier.anim_cooldown = 0.0f;

    parachute.tx = &texture_holder.parachute;
    parachute.rotation = 0;
        
    rl.tx = &texture_holder.launcher;
    rl.rotation = 0;

    rockets.next = NULL;
    particles.next = NULL;

    for (int i = 0; i < NUM_PLATFORMS; i++)
        platforms[i].tx = &texture_holder.platform;

    pickup.tx = &texture_holder.pickup[0];
    pickup.id = pickup_parachute;

    new_healthpack.tx = &texture_holder.healthpack;
    new_healthpack.x = -100;
    new_healthpack.y = -100;

    health_hud.tx = &texture_holder.hud;
    health_hud.x = 5;
    health_hud.y = screen_height - texture_holder.hud.height - 5;
    sprintf(health_hud.text, "%d", health_base);

    pickup_hud.tx = &texture_holder.hud;
    pickup_hud.x = screen_width - texture_holder.hud.width - 5;
    pickup_hud.y = screen_height - texture_holder.hud.height - 5;
    strcpy(pickup_hud.text, "EMPTY");

    try_again_button.tx = &texture_holder.button[0];
    try_again_button.x = screen_width / 2 - texture_holder.button[0].width / 2;
    try_again_button.y = 500;
    try_again_button.state = button_normal;
    strcpy(try_again_button.text, "TRY AGAIN");

    for (int i = 0; i < NUM_MUSIC; i++)
        PlayMusicStream(music[i]);

    restart();
}

void input(void) {
    volume_control();

    if (!movement_allowed)
        return;

    if (IsKeyDown(key_move_right) && !IsKeyDown(key_move_left)) {
        red_soldier.x += speed_walking * dt;
        red_soldier.state = state_walking;

        if (red_soldier.pickup_active == pickup_parachute && parachute.rotation > -30)
            parachute.rotation -= 60 * dt;
    } else if (IsKeyDown(key_move_left) && !IsKeyDown(key_move_right)) {
        red_soldier.x -= speed_walking * dt;
        red_soldier.state = state_walking;
    
        if (red_soldier.pickup_active == pickup_parachute && parachute.rotation < 30)
            parachute.rotation += 60 * dt;
    } else
        red_soldier.state = state_standing;
    if (red_soldier.speed_y < -100 || red_soldier.speed_y > 100)
        red_soldier.state = state_jumping;

    if (!IsKeyDown(key_move_left) && !IsKeyDown(key_move_right))
        parachute.rotation += (parachute.rotation > 0) ? (-100 * dt) : (100 * dt);
    if (IsKeyDown(key_jump) && !red_soldier.falling) {
        PlaySound(sfx[sfx_jump]);
        if (red_soldier.pickup_active == pickup_parachute) {
            red_soldier.gravity_factor = 1;
            red_soldier.pickup_active = pickup_none;
        }
        red_soldier.speed_y = -accel_jump;
    }

    if ((IsMouseButtonPressed(button_shoot) || IsKeyPressed(key_shoot_alt)) && red_soldier.rl_cooldown < 0.0f) {
        red_soldier.rl_cooldown = 0.8f;

        spawn_rocket();
    }

    if (IsKeyPressed(key_use_pickup)) {
        red_soldier.pickup_active = red_soldier.pickup;
        red_soldier.pickup = pickup_none;
        switch (red_soldier.pickup_active) {
            case pickup_parachute:
                red_soldier.gravity_factor = pickup_gravity_factor;
                break;

            case pickup_crit:
                red_soldier.rl_knockback_factor = pickup_rl_knockback_factor;
                red_soldier.color = RED;    
                break; 
        }
    }
}

void load_assets(void) {
    Image image;

    LOAD_TEXTURE_ARRAY(red_soldier, 6, 5);
    LOAD_TEXTURE_ARRAY(pickup, num_pickup, 8);
    LOAD_TEXTURE_ARRAY(button, 2, 8);
    LOAD_TEXTURE_ARRAY(bg, NUM_BG, 5.f * (float)screen_height / 1080.f);

    LOAD_TEXTURE(red_soldier_jumping, 5);
    LOAD_TEXTURE(rocket, 3);
    LOAD_TEXTURE(launcher, 5);
    LOAD_TEXTURE(particle_smoke, 15);
    LOAD_TEXTURE(platform, 5);
    LOAD_TEXTURE(parachute, 5);
    LOAD_TEXTURE(hud, 5);
    LOAD_TEXTURE(healthpack, 6);

    UnloadImage(image);

    for (int i = 0; i < num_sfx; i++) {
        char name[16];
        sprintf(name, "sfx%d.ogg", i);
        sfx[i] = LoadSound(path_to_file(name));
    }

    for (int i = 0; i < NUM_MUSIC; i++) {
        char name[16];
        sprintf(name, "music%d.ogg", i);
        music[i] = LoadMusicStream(path_to_file(name));
    }
}

void manage_rockets(void) {
    for (Rocket *r = &rockets; r->next != NULL; r = r->next) {
        rocket_border_check(r->next);

        if (r->next->collided) {
            if (r->next->should_explode) {
                PlaySound(sfx[sfx_explosion]);

                spawn_particle(r->next);

                Rocket rocket = *r->next;
                if (abs(red_soldier.x + MIDDLE_X(red_soldier) - r->next->x - MIDDLE_X(rocket)) < 200
                && abs(red_soldier.y + MIDDLE_Y(red_soldier) - r->next->y - MIDDLE_Y(rocket)) < 200
                && game_state != game_over) {
                    red_soldier.speed_x += red_soldier.rl_knockback_factor * -1 * r->next->speed_x;
                    red_soldier.speed_y += red_soldier.rl_knockback_factor * -1 * r->next->speed_y; 
                
                    if (game_state == game_active) {
                        red_soldier.hp -= 20 * red_soldier.rl_knockback_factor;
                        if (red_soldier.hp <= 0)
                            end_game(&game_state, &sfx[sfx_death], music);
                    }
                }
                
                if (red_soldier.pickup_active == pickup_crit) {
                    red_soldier.rl_knockback_factor = 1;
                    red_soldier.color = WHITE;
                    red_soldier.pickup_active = pickup_none;
                }
            }
            
            Rocket *r_next = r->next->next;
            free(r->next);
            r->next = r_next;
            break;
        }
    }
}

char *path_to_file(const char *name) {
    char *path = malloc(sizeof(char) * strlen(directory) + strlen(name) + 1);
    sprintf(path, "%s%s", directory, name);
    return path;
}

bool pickup_collect_check(Pickup *p, Soldier *r) {
    if (r->x + r->tx->width > p->x && r->x < p->x + p->tx->width) {
        if (r->y + r->tx->height > p->y && r->y < p->y + p->tx->height) {
            if (r->pickup == 0) {
                r->pickup = p->id;
                p->x = -100;
                p->y = -100;
                return 1;
            }
        }
    }
    return 0;
}

void platform_collision_check_rocket(Platform *p, Rocket *r) {
    if (r->x + r->tx->width > p->x && r->x < p->x + p->tx->width) {
        if (r->y + r->tx->height > p->y 
        && r->y < p->y + p->tx->height)
            r->collided = 1;
    } 
}

void platform_collision_check_soldier(Platform *p, Soldier *s) {
    if (s->x + s->tx->width > p->x && s->x < p->x + p->tx->width) {
        if (s->y + s->tx->height > p->y && s->y + s->tx->height < p->y + p->tx->height) {
            s->y = p->y - s->tx->height;
            s->speed_y = 0;
            s->falling = 0;
        }
    }
}

void restart(void) {
    game_state = game_menu;
    level = 1;
    score = 0;

    red_soldier.color = WHITE;
    red_soldier.x = (int)(screen_width / 2) - red_soldier.tx->width;
    red_soldier.y = screen_height - red_soldier.tx->height; 
    red_soldier.speed_x = 0;
    red_soldier.speed_y = 0;
    red_soldier.falling = 0;
    red_soldier.pickup = pickup_none;
    red_soldier.pickup_active = pickup_none;
    red_soldier.state = state_standing;
    red_soldier.gravity_factor = 1;
    red_soldier.rl_knockback_factor = 1;
    red_soldier.hp = health_base;

    pickup.x = -100;
    pickup.y = -100;

    for (int i = 0; i<NUM_HEALTHPACKS; i++)
        healthpacks[i] = new_healthpack;

    for (int i = 0; i < 2; i++) {
        bg[i].y = -i * screen_height;
        bg[i].tx = &texture_holder.bg[i]; 
    }

    for (int i = 0; i < NUM_PLATFORMS; i++) {
        platforms[i].x = RANDOM_PLATFORM_X;
        platforms[i].y = screen_height - (i + 1) * 1000 / NUM_PLATFORMS;
    } 

    for (int i = 0; i < NUM_HEALTHPACKS; i++)
        healthpacks[i] = new_healthpack;
}

void rocket_border_check(Rocket *r) {
    if (r->y + r->tx->height >= screen_height)
        r->collided = 1;
    else if (r->x <= 0
    || r->x >= screen_width
    || r->y <= 0) {
        r->collided = 1;
        r->should_explode = 0;
    }
}

void run(void) {
    while (!WindowShouldClose()) {
        dt = GetFrameTime();
        mouse = GetMousePosition();

        manage_rockets();

        update_score();
        update_music();

        shift = red_soldier.speed_y * dt;
        should_shift = red_soldier.y == SCREEN_MIDDLE(red_soldier) && red_soldier.speed_y < 0;
        movement_allowed = game_state != game_over;

        ClearBackground(BLACK);
        BeginDrawing();

        update_bg();
        update_pickup();
        update_healthpacks();
        update_parachute();
        gravity();
        update_platforms();
        input();
        update_rockets();
        update_soldier();
        update_rl();
        update_particles();
        update_hud();

        EndDrawing();
    }
}

void soldier_border_check(Soldier *s) {
    if (s->x < 0)
        s->x = 0;
    else if (s->x + s->tx->width > screen_width)
        s->x = screen_width - s->tx->width;
}

void spawn_healthpack(int x, int y) {
    for (int j = 0; j < NUM_HEALTHPACKS; j++) {
        if (!IS_VISIBLE(healthpacks[j])) {
            healthpacks[j].x = x - MIDDLE_X(healthpacks[j]);
            healthpacks[j].y = y - healthpacks[j].tx->height;
            break;
        }
    }
}

void spawn_particle(Rocket *r) {
    Particle *p = &particles;

    while (p->next != NULL)
        p = p->next;
    p->next = malloc(sizeof(Particle));
    p = p->next;

    p->tx = &texture_holder.particle_smoke;
    p->x = r->x;
    p->y = r->y;
    p->rotation = rand() % 360;
    p->alpha = 255;
    p->next = NULL;
}

void spawn_pickup(int x, int y) {
    pickup.id = rand() % num_pickup + 1;
    pickup.tx = &texture_holder.pickup[pickup.id - 1];
    pickup.x = x - MIDDLE_X(pickup); 
    pickup.y = y - pickup.tx->height; 
}

void spawn_rocket(void) {
    Rocket *r = &rockets;

    while (r->next != NULL)
        r = r->next;
    r->next = malloc(sizeof(Rocket));
    r = r->next;

    r->tx = &texture_holder.rocket;
    r->x = red_soldier.x + MIDDLE_X(red_soldier);
    r->y = red_soldier.y + MIDDLE_Y(red_soldier) / 4;
    r->rotation = 90 - atan2((red_soldier.x + MIDDLE_X(red_soldier) - mouse.x), (red_soldier.y + MIDDLE_Y(red_soldier) - mouse.y)) * 180 / PI;
    r->speed_x = -rocket_max_speed * cos(r->rotation * PI / 180);
    r->speed_y = -rocket_max_speed * sin(r->rotation * PI / 180);
    r->collided = 0;
    r->should_explode = 1;
    r->next = NULL;
}

void unload_assets(void) {
    for (int i = 0; i < 6; i++)
        UnloadTexture(texture_holder.red_soldier[i]);
    UnloadTexture(texture_holder.rocket);
    UnloadTexture(texture_holder.launcher);
    UnloadTexture(texture_holder.parachute);
    UnloadTexture(texture_holder.platform);
    for (int i = 0; i < num_pickup; i++)
        UnloadTexture(texture_holder.pickup[i]);
    UnloadTexture(texture_holder.healthpack);
    UnloadTexture(texture_holder.hud);
    for (int i = 0; i < 2; i++)
        UnloadTexture(texture_holder.button[i]);
    UnloadTexture(texture_holder.particle_smoke);
    for (int i = 0; i < NUM_BG; i++)
        UnloadTexture(texture_holder.bg[i]);

    for (int i = 0; i < num_sfx; i++)
        UnloadSound(sfx[i]);

    for (int i = 0; i < NUM_MUSIC; i++)
        UnloadMusicStream(music[i]);
}

void update_bg(void) {
    for (int i = 0; i < 2; i++) {
        if (bg[i].y > screen_height) {
            bg[i].y = -screen_height;
            bg[1-i].y = 0;

            level++;
            if (level > NUM_BG - 1) level = 7;
            bg[i].tx = &texture_holder.bg[level];
        } 
        if (should_shift)
            bg[i].y -= shift / 2;

        DrawTexture(*bg[i].tx, 0, bg[i].y, WHITE);
    }
}

void update_healthpacks(void) {
    for (int i = 0; i < NUM_HEALTHPACKS; i++) {
        if (IS_VISIBLE(healthpacks[i])) {
            DRAW(healthpacks[i]);
            if (COLLISION(healthpacks[i], red_soldier)) {
                red_soldier.hp += heal_amount;
                healthpacks[i] = new_healthpack;
            }
        }
        if (should_shift)
            healthpacks[i].y -= shift;     
    }
}

void update_hud(void) {
    switch (game_state) {
        case game_menu:
            draw_text_center("ROCKETMAN ADVENTURES", 200, 100, WHITE);
            draw_text_center(version, 300, 64, WHITE); 
            draw_text_center("START JUMPING TO BEGIN", 400, 64, WHITE);
            break;
        case game_active:
            if (red_soldier.hp < health_base / 4)
                health_hud.text_color = text_color[col_low];
            else if (red_soldier.hp > health_base)
                health_hud.text_color = text_color[col_high];
            else
                health_hud.text_color = text_color[col_normal];

            sprintf(health_hud.text, "%d", red_soldier.hp);
            DRAW(health_hud);
            draw_text(health_hud.text, health_hud.x + 40, health_hud.y + 30, 100, health_hud.text_color); 
           
            DRAW_PRO(pickup_hud, -1, 1, 0, 0, 0, WHITE);
            if (red_soldier.pickup != pickup_none)
                DrawTexture(texture_holder.pickup[red_soldier.pickup - 1], pickup_hud.x + 150, pickup_hud.y + 25, WHITE);
            else
                draw_text(pickup_hud.text, pickup_hud.x + 65, pickup_hud.y + 40, 64, WHITE);

            draw_text("SCORE:", 10, 10, 64, WHITE);
            draw_text(score_string, 250, 10, 64, WHITE);
            break;
        case game_over:
            if (MOUSE_HOVER_BUTTON(try_again_button,mouse)) {
                try_again_button.state = button_hover;
                if (IsMouseButtonPressed(button_shoot))
                    restart();
            } else
                try_again_button.state = button_normal;

            DrawRectangle(0, 0, screen_width, screen_height, (Color){ 0, 0, 0, 150 });
            draw_text_center("GAME OVER", 200, 100, WHITE);
            draw_text_center("SCORE:", 300, 64, WHITE);
            draw_text_center(score_string, 375, 64, WHITE);
            DrawTexture(try_again_button.tx[try_again_button.state], try_again_button.x, try_again_button.y, WHITE);
            draw_text(try_again_button.text, try_again_button.x + 12, try_again_button.y + 45, 64, WHITE);
            break;
        default:
            draw_text("ERROR", 100, 100, 120, WHITE);
    } 
}

void update_music(void) {
    switch (game_state) {
        case game_menu:
            UpdateMusicStream(music[music_menu]);
            break;
        case game_active:
            if (level < 7)
                UpdateMusicStream(music[music_normal]);
            else
                UpdateMusicStream(music[music_space]);
            break;
    }
}

void update_parachute(void) {
    if (red_soldier.gravity_factor < 1) {
        DrawTexturePro(
            *parachute.tx,
            (Rectangle){
                .x = 0,
                .y = 0,
                .width = parachute.tx->width,
                .height = parachute.tx->height
            },
            (Rectangle){
                .x = red_soldier.x + MIDDLE_X(red_soldier),
                .y = red_soldier.y + 10,
                .width = parachute.tx->width,
                .height = parachute.tx->height
            },
            (Vector2){
                .x = (int)(parachute.tx->width / 2),
                .y = parachute.tx->height
            },
            parachute.rotation,
            WHITE
        ); 
    }
}

void update_particles(void) {
    for (Particle *p = &particles; p->next != NULL; p = p->next) {
        Particle particle = *p->next;

        if (p->next->alpha < 5 || !IS_VISIBLE(particle)) {
            Particle *p_next = p->next->next;
            free(p->next);
            p->next = p_next;
            break;
        }

        if (should_shift)
            p->next->y -= shift;

        p->next->alpha -= 2 * dt;
        
        Color color = { .r = 255, .g = 255, .b = 255, .a = p->next->alpha };
        DRAW_PRO(particle, 1, 1, particle.rotation, MIDDLE_X(particle), MIDDLE_Y(particle), color);
    }
}

void update_pickup(void) {
    if (pickup_collect_check(&pickup, &red_soldier))
        PlaySound(sfx[sfx_pickup]);
    if (should_shift)
        pickup.y -= shift; 
    if (IS_VISIBLE(pickup))
        DRAW(pickup);
}

void update_platforms(void) {
    for (int i = 0; i < NUM_PLATFORMS; i++) {
        if (red_soldier.speed_y > 0)
            platform_collision_check_soldier(&platforms[i], &red_soldier);

        if (should_shift)
            platforms[i].y -= shift;

        for (Rocket *r = &rockets; r->next != NULL; r = r->next)
            platform_collision_check_rocket(&platforms[i], r->next);

        if (platforms[i].y > screen_height) {
            platforms[i].x = RANDOM_PLATFORM_X;
            platforms[i].y = -platforms[i].tx->height;
            
            int random = rand() % 10;
            if (random == 0 && !IS_VISIBLE(pickup))
                spawn_pickup(platforms[i].x + MIDDLE_X(platforms[i]), platforms[i].y);
            else if (random > 7)
                spawn_healthpack(platforms[i].x + MIDDLE_X(platforms[i]), platforms[i].y);
        }

        DRAW(platforms[i]);
    }
}

void update_rl(void) {
    if (movement_allowed) {
        rl.rotation = 270 - atan2((red_soldier.x + MIDDLE_X(red_soldier) - mouse.x), (red_soldier.y + MIDDLE_Y(red_soldier) - mouse.y)) * 180 / PI; 
        if (mouse.x < red_soldier.x + MIDDLE_X(red_soldier)) {
            red_soldier.flip = -1;
            rl.x = red_soldier.x + 40;
        } else {
            red_soldier.flip = 1;
            rl.x = red_soldier.x + 25;
        }
        rl.y = red_soldier.y + 45;
    }

    DRAW_PRO(rl, 1, red_soldier.flip, rl.rotation, 50, 45, red_soldier.color);
}

void update_rockets(void) {
    for (Rocket *r = &rockets; r->next != NULL; r = r->next) {
        Rocket rocket = *r->next;
        DRAW_PRO(rocket, 1, 1, rocket.rotation, MIDDLE_X(rocket), MIDDLE_Y(rocket), red_soldier.color);
        r->next->x += r->next->speed_x * dt;
        r->next->y += r->next->speed_y * dt;
    }
}

void update_score(void) {
    if (!movement_allowed)
        return;

    if (red_soldier.y < SCREEN_MIDDLE(red_soldier)) {
        score -= red_soldier.speed_y * dt;
        sprintf(score_string, "%d", score);

        red_soldier.y = SCREEN_MIDDLE(red_soldier); 
   
        if (game_state == game_menu)
            game_state = game_active;
    } 
}

void update_soldier(void) {
    if (movement_allowed) {
        red_soldier.x += red_soldier.speed_x * dt;
        if (red_soldier.speed_y > 0)
            red_soldier.y += red_soldier.speed_y * dt * red_soldier.gravity_factor; 
        else
            red_soldier.y += red_soldier.speed_y * dt; 

        red_soldier.speed_x += (red_soldier.speed_x > 0) ? -8 : 8;
        if (red_soldier.speed_x > -5 && red_soldier.speed_x < 5)
            red_soldier.speed_x = 0;

        soldier_border_check(&red_soldier);

        red_soldier.rl_cooldown -= dt;  
    }

    switch (red_soldier.state) {
        case state_standing:
            red_soldier.tx = &texture_holder.red_soldier[0];
            break;

        case state_walking:
            red_soldier.anim_cooldown -= dt;
            if (red_soldier.anim_cooldown < 0.0f) {
                red_soldier.frame++;
                red_soldier.tx = &texture_holder.red_soldier[red_soldier.frame % 6];
                red_soldier.anim_cooldown = 0.1;
            }
            break;

        case state_jumping:
            red_soldier.tx = &texture_holder.red_soldier_jumping;
            break;
    }
    DRAW_PRO(red_soldier, red_soldier.flip, 1, 0, 0, 0, red_soldier.color);
}

void volume_control(void) {
    if (IsKeyPressed(key_vol_up) && volume <= 0.9f) {
        volume += 0.1f;
        SetMasterVolume(volume);
    } else if (IsKeyPressed(key_vol_down) && volume >= 0.1f) {
        volume -= 0.1f;
        SetMasterVolume(volume);
    } else if (IsKeyPressed(key_mute)) {
        muted =! muted;
        SetMasterVolume(muted ? 0 : volume);
    }
}

int main(void) {
    init();
    run();
    unload_assets();
    close();   

    return 0;
}
