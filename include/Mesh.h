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

typedef struct M
{
    BvhNode** nodeToGeomBkPtr;
    unsigned int numGeometries, maxGeometries;
    G* geometries;
    //pointer to vertex data
    Vec3 *positions, *normals;
    Vec2 *texCords;
} M;

Mesh meshGen(unsigned int numGeometries);
unsigned int meshGenGeometry(Mesh* mesh, unsigned int numTriangles, Vec3 pos);
unsigned int meshGeomAddTri(Mesh* mesh, unsigned int geomIdx, Triangle triangle);


M* meshLoadOBJ(const char *filePath);
unsigned int meshAddGeom(M* mesh, G* geom);

//insert to BVH
void meshInsertToBvh(Mesh* mesh, BvhNode* bvh);

Geometry* meshGetGeometry(Mesh* mesh, unsigned int meshIdx);
BvhNode* mGetBvhPtr(Mesh* mesh, unsigned int meshIdx);

#endif