#ifndef __RAY_H__
#define __RAY_H__
#include "Defines.h"
#include "Maths.h"
#include <string.h>

typedef struct {
  Vec3 origin;
  Vec3 direction;
  float closestDistance;
  float distance;
} Ray;

Ray rayInit(Vec3 origin, Vec3 direction, float distance);

#endif
