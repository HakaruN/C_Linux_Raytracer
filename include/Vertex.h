#ifndef __VERTEX_H__
#define __VERTEX_H__

#include "Maths.h"
#include <string.h>
typedef struct{
  Vec3 position;
  Vec3 normal;
  Vec3 colour;
  Vec2 texCords;
} Vertex;


Vertex vertexGen(Vec3 position, Vec3 normal, Vec3 colour, Vec2 texCords);


#endif
