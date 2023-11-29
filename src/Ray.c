#include "../include/Ray.h"

#ifdef FX
Ray rayInit(Vec3 origin, Vec3 direction, UFX16_16 distance)
#else
  Ray rayInit(Vec3 origin, Vec3 direction, float distance)
#endif
{
  Ray r;
  #ifdef FX
    memcpy(r.origin, origin, 3*sizeof(SFX16_16));
    memcpy(r.direction, direction, 3*sizeof(SFX16_16)); 
  #else
    memcpy(r.origin, origin, 3*sizeof(float));
    memcpy(r.direction, direction, 3*sizeof(float));
  #endif
  r.distance = distance;
  return r;
}
