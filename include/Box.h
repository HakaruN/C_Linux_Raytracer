#ifndef __BOX_H__
#define __BOX_H__

#include "Maths.h"

typedef Vec3 BBox[2];
BBox* genBox(Vec3 min, Vec3 max);
#endif
