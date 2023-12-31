#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "structs.h"

char *path_to_file(char *str);
void rocket_border_check(Rocket *r);
void soldier_border_check(Soldier *s);
void platform_collision_check_soldier(Platform *p, Soldier *s);
void platform_collision_check_rocket(Platform *p, Rocket *r);
bool pickup_collect_check(Pickup *p, Soldier *r);
void draw_text_full(const char *text, int x, int y, int font_size, Color color);
void draw_text_full_center(const char *text, int y, int font_size, Color color);

#endif
