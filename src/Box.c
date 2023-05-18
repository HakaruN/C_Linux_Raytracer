#include "../include/Box.h"


inline BBox* genBox(Vec3 min, Vec3 max)
{
  BBox* b = malloc(sizeof(BBox));;
  b[0] = min;
  b[1] = max;
  return b;
}
