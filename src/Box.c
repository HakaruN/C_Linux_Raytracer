#include "../include/Box.h"


inline BBox* genBox(Vec3 min, Vec3 max)
{
  BBox* b = malloc(sizeof(BBox));
  *b[0][0] = min[0];
  *b[0][1] = min[1];
  *b[0][2] = min[2];
  *b[1][0] = max[0];
  *b[1][1] = max[1];
  *b[1][2] = max[2];
  return b;
}
