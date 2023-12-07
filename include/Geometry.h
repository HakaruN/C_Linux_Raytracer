#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include "../include/Triangle.h"
#include "../include/Box.h"
#include "../vendor/fast_obj/fast_obj.h"


#define X 0
#define Y 1
#define Z 2
#define VERT1 0
#define VERT2 1
#define VERT3 2
#define VERT4 3
#define FLOATS_PER_POS 3
#define FLOATS_PER_NORM 3
#define FLOATS_PER_TEX 2


typedef struct Geometry{
    Triangle* triangles;//Array of triangles in the Geometry. The BVH stores pointers to these triangles.
    unsigned int maxTriangles, numTriangles;//Number of triangles we have space allocated for in the arrays, num triangles actually allocated in the array.
    Vec3 position;//position of the Geometry in 3D space
    Mat4 transform;
    unsigned long geometryID;//unique ID of this geometry
} Geometry;

typedef struct G{
    T* triangles;
    //pointer to the arrays stored in the geom
    Vec3 *positions, *normals;
    Vec2 *texCords;
    unsigned int maxTriangles, numTriangles;//Number of triangles we have space allocated for in the arrays, num triangles actually allocated in the array.
    //Vec3 position;//position of the Geometry in 3D space
    //Mat4 transform;
    //unsigned long geometryID;//unique ID of this geometry
} G;

Geometry genGeometry(unsigned int numTriangles, Vec3 position);
G* genG(Vec3 *pPositions, Vec3 *pNormals, Vec2 *pTexCords, unsigned int maxTriangles);//when we need to allocate a new G
unsigned int Ginit(G* geom, Vec3 *pPositions, Vec3 *pNormals, Vec2 *pTexCords, unsigned int maxTriangles);//when we already have a G allocated

unsigned int geomAddTriangle(Geometry* geometry, Triangle triangle);
unsigned int GAddTriangle(G* geometry, T* triangle);


Triangle* geomGetTriangle(Geometry* geometry, unsigned int index);

BBox* geomGenAABB(Geometry* geometry);

void GeometryFree(Geometry* geom);



#endif