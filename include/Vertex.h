#ifndef __VERTEX_H__
#define __VERTEX_H__

#include "Maths.h"
#include <string.h>
typedef struct Vertex{
  Vec3 position;//Position of the vertex with respect to the triangle
  Vec3 transformedPosition;//Global position of the vertex (vert pos after transform with tri pos)
  Vec3 normal;
  Vec3 colour;
  Vec2 texCords;
} Vertex;


typedef struct{
  unsigned int size;//how many verts we can fit in the array 
  unsigned int numVerts;//num verts currently in the struct
  Vertex* vertices;//actuall array of the verts
} Vertices;

typedef struct V{
  Vec3* positions;
  Vec3* normals;
  Vec2* texCoords;
  unsigned int posIndex;
  unsigned int normalIndex;
  unsigned int texCordIndex;
} V;

Vertex vertexGen(Vec3 position, Vec3 normal, Vec3 colour, Vec2 texCords);
Vertices verticesGen(unsigned int size);//gen an a vertex buffer object. Entries here are to be accessed via indices
void verticesAddVert(Vertices* verts,Vertex v);
Vertex* verticesGetVert(Vertices* verts, unsigned int vId);

#endif
