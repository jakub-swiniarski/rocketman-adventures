#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "structs.h"

char *pathToFile(char *str);
void rocketBorderCheck(Rocket *r);
void soldierBorderCheck(Soldier *s);
void platformCollisionCheckS(Platform *p, Soldier *s);
void platformCollisionCheckR(Platform *p, Rocket *r);
bool pickupVisible(Pickup *p);
void pickupCollectCheck(Pickup *p, Soldier *r);

#endif
