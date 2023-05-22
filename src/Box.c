#include "../include/Box.h"


inline BBox* genBox(Vec3 min, Vec3 max)
{
  BBox* b = malloc(sizeof(BBox));
  #ifdef DEBUG
  printf("Generating box\n");
  printVec3(min);
  printVec3(max);
#endif
  memcpy(b->min, min, 3 * sizeof(float));
  memcpy(b->max, max, 3 * sizeof(float));
  return b;
}
