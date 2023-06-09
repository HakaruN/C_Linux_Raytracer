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


typedef struct{
  unsigned int size;//how many verts we can fit in the array
  unsigned int numVerts;//num verts currently in the struct
  Vertex* vertices;//actuall array of the verts
} Vertices;

Vertex vertexGen(Vec3 position, Vec3 normal, Vec3 colour, Vec2 texCords);
Vertices verticesGen(unsigned int size);//gen an a vertex list. Entries here are to be accessed via indices
void verticesAddVert(Vertices* verts, Vertex v);
Vertex* verticesGetVert(Vertices* verts, unsigned int vId);

#endif
