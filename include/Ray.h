#ifndef __RAY_H__
#define __RAY_H__
#include "Defines.h"
#include "Maths.h"
#include <string.h>

typedef struct {
  Vec3 origin;
  Vec3 direction;
  #ifdef FX
    UFX16_16 distance;
  #else
    float distance;
  #endif
} Ray;

#ifdef FX
  Ray rayInit(Vec3 origin, Vec3 direction, UFX16_16 distance);
#else
  Ray rayInit(Vec3 origin, Vec3 direction, float distance);
#endif

#endif
