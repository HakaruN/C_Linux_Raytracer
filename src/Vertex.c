#include "../include/Vertex.h"


Vertex vertexGen(Vec3 position, Vec3 normal, Vec3 colour, Vec2 texCords)
{
  Vertex v;

    memcpy(v.position, position, sizeof(Vec3));
    memcpy(v.transformedPosition, position, sizeof(Vec3));
    memcpy(v.normal, normal, sizeof(Vec3));
    memcpy(v.colour, colour, sizeof(Vec3));
    memcpy(v.texCords, texCords, sizeof(Vec2));
  return v;
}

Vertices verticesGen(unsigned int size)
{
  Vertices v;
  v.vertices = malloc(size * sizeof(Vertex));
  v.size = size;
  v.numVerts = 0;
  return v;
}

void verticesAddVert(Vertices* verts, Vertex v)
{
  ///check if we have space to add the new vert
  //if not, gen bigger buffer, copy contents
  if(verts->numVerts >= verts->size){
    unsigned int newSize = verts->size + (verts->size / 10) + 1;//increase by ~10% (+1 because for small arrays; 10% < 1 so wouldn't increase the size)
    Vertex* vtmp = malloc(newSize * sizeof(Vertex));
    verts->size = newSize;
    memcpy(vtmp, verts->vertices, verts->numVerts * sizeof(Vertex));
    free(verts->vertices);
    verts->vertices = vtmp;
  }

  //add the vertex
  verts->vertices[verts->numVerts] = v;
  verts->numVerts++;
}

inline Vertex* verticesGetVert(Vertices* verts, unsigned int vId)
{
  printf("hello\n");
  if(vId >= 0 && vId < verts->numVerts)
    return &verts->vertices[vId];
  else
    return NULL;
}
