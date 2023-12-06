#ifndef __MESH_H__
#define __MESH_H__

#include "Geometry.h"
#include "BvhNode.h"

typedef struct Mesh
{
    BvhNode** nodeToGeomBkPtr;
    Geometry* geometries;
    unsigned int numGeometries, maxGeometries;
    Mat4 transform;
} Mesh;

Mesh meshGen(unsigned int numGeometries);
unsigned int meshGenGeometry(Mesh* mesh, unsigned int numTriangles, Vec3 pos);

unsigned int meshGeomAddTri(Mesh* mesh, unsigned int geomIdx, Triangle triangle);

//insert to BVH
void meshInsertToBvh(Mesh* mesh, BvhNode* bvh);

Geometry* meshGetGeometry(Mesh* mesh, unsigned int meshIdx);
BvhNode* mGetBvhPtr(Mesh* mesh, unsigned int meshIdx);

#endif