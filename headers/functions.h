#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "structs.h"

char *pathToFile(char *str);
void rocketBorderCheck(Rocket *r);
void soldierBorderCheck(Soldier *s);
void platformCollisionCheck(Platform *p, Soldier *s);

#endif
