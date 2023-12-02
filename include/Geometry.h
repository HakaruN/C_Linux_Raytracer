#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include "../include/Triangle.h"
#include "../include/Box.h"


typedef struct Geometry{
    Triangle* triangles;//Array of triangles in the Geometry. The BVH stores pointers to these triangles.
    unsigned int maxTriangles, numTriangles;//Number of triangles we have space allocated for in the arrays, num triangles actually allocated in the array.
    Vec3 position;//position of the Geometry in 3D space
    unsigned long geometryID;//unique ID of this geometry
} Geometry;

Geometry genGeometry(unsigned int numTriangles, Vec3 position);
unsigned int geomAddTriangle(Geometry* geometry, Triangle triangle);

Triangle* geomGetTriangle(Geometry* geometry, unsigned int index);

BBox* geomGenAABB(Geometry* geometry);

void GeometryFree(Geometry* geom);



#endif