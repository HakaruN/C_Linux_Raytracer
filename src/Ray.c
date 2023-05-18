#include "../include/Ray.h"

Ray rayInit(Vec3 origin, Vec3 direction, float distance)
{
  Ray r;
  memcpy(r.origin, origin, 3*sizeof(float));
  memcpy(r.direction, direction, 3*sizeof(float));
  //  r.origin = origin;
  //  r.direction = direction;
  r.distance = distance;
  return r;
}
