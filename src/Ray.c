#include "../include/Ray.h"

Ray rayInit(Vec3 origin, Vec3 direction, float distance)
{
  Ray r;
  memcpy(r.origin, origin, sizeof(Vec3));
  memcpy(r.direction, direction, sizeof(Vec3));
  r.distance = distance;
  return r;
}
