#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "structs.h"

char *pathToFile(char *str);
void rocketBorderCheck(Rocket *r);
void soldierBorderCheck(Soldier *s);
void platformCollisionCheckS(Platform *p, Soldier *s);
void platformCollisionCheckR(Platform *p, Rocket *r);
bool pickupCollectCheck(Pickup *p, Soldier *r);
void drawTextFull(const char *text, int x, int y, int fontSize, Color color);
void drawTextFullCenter(const char *text, int y, int fontSize, Color color);

#endif
