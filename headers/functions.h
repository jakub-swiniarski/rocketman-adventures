#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "structs.h"

char *pathToFile(char *str);
bool rocketCollisionCheck(Rocket *r);
void soldierBorderCheck(Soldier *s);
void platformCollisionCheck(Platform *p, Soldier *s);

#endif
