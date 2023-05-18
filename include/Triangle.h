#ifndef __TRIANGLE_H__
#define __TRIANGLE_H__

#include "Vertex.h"
#include "Ray.h"
#include <math.h>
#include <stdlib.h>


typedef struct{
  Vertex verts[3];
  //  Vertex v0, v1, v2;
  Vec3 pos;
  unsigned char* tex;
} Triangle;

Triangle triangleGen(Vertex* verts, Vec3 Pos);
void freeTriangle(Triangle* triangle);
void barycentricCoords(float* out, Vec3 vert0, Vec3 vert1, Vec3 vert2, Vec3 point);
int triangleIntersect(Vec3 v0, Vec3 v1, Vec3 v2, Ray* ray,  Vec3 intersectionPoint);




#endif
