#ifndef __TRIANGLE_H__
#define __TRIANGLE_H__

#include "Vertex.h"
#include "Texture.h"
#include "Ray.h"
#include <math.h>
#include <stdlib.h>


typedef struct Triangle{
  Vertex verts[3];
  Vec3 position;
  Texture* texture;
} Triangle;

Triangle triangleGen(Vertex* verts, Vec3 Pos, Texture* texture);

//delete triangle
//void triDeleteTriangle()
void freeTriangle(Triangle* triangle);
void barycentricCoords(Vec3 out, Vec3 vert0, Vec3 vert1, Vec3 vert2, Vec3 point);
int triangleIntersect(Vec3 v0, Vec3 v1, Vec3 v2, Ray* ray, Vec3 intersectionPoint);




#endif
