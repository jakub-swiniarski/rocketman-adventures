#ifndef CONFIG_H
#define CONFIG_H

/* screen resolution */
static const int screen_width = 1920;
static const int screen_height = 1080;
#define FULLSCREEN /* comment this out for windowed mode */

/* frames per second */
static const int fps = 60;

/* sound */
static float volume = 1.0f;
static bool muted = 0;

/* appearance */
static const Color text_color[3] = { RED, WHITE, GREEN }; /* low, normal, high */

/* keyboard controls */
static const KeyboardKey key_move_left = KEY_A;
static const KeyboardKey key_move_right = KEY_D;
static const KeyboardKey key_jump = KEY_SPACE;
static const KeyboardKey key_shoot_alt = KEY_R;
static const KeyboardKey key_use_pickup = KEY_Q;
static const KeyboardKey key_mute = KEY_F1;
static const KeyboardKey key_vol_up = KEY_F3;
static const KeyboardKey key_vol_down = KEY_F2;

/* mouse controls */
static const MouseButton button_shoot = MOUSE_LEFT_BUTTON;

/* gameplay */
#define NUM_PLATFORMS 10
#define NUM_HEALTHPACKS 2
static const int accel_gravity = 1000;
static const int accel_jump = 400;
static const int heal_amount = 50;
static const int health_base = 200;
static const int rocket_max_speed = 960;
static const int speed_walking = 150;
static const float pickup_gravity_factor = 0.2;
static const float pickup_rl_knockback_factor = 2.f;
static const float rl_cooldown = 0.8f;

#endif 
