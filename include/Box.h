#ifndef __BOX_H__
#define __BOX_H__

#include "Maths.h"
#include "../include/Ray.h"

//typedef Vec3 BBox[2];
typedef struct BBox {
  Vec3 min;
  Vec3 max;
} BBox;
BBox* genBox(Vec3 min, Vec3 max);
int rayBoxIntersection(Ray* ray, BBox* box);
#endif
