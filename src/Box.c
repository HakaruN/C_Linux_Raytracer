#include "../include/Box.h"


inline BBox* genBox(Vec3 min, Vec3 max)
{
  BBox* b = (BBox*)malloc(sizeof(BBox));
  #ifdef DEBUG
  printf("Generating box\n");
  printVec3(min);
  printVec3(max);
#endif
  memcpy(b->min, min, sizeof(Vec3));
  memcpy(b->max, max, sizeof(Vec3));
  return b;
}
