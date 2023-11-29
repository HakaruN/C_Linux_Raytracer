#include "../include/Ray.h"

Ray rayInit(Vec3 origin, Vec3 direction, float distance)
{
  Ray r;
  memcpy(r.origin, origin, 3*sizeof(float));
  memcpy(r.direction, direction, 3*sizeof(float));
  r.distance = distance;
  r.closestDistance = distance;
  return r;
}
