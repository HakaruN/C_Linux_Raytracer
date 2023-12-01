#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include "../include/Triangle.h"
#include "../include/BvhNode.h"

static unsigned long geomCtr;

typedef struct {
    Triangle* triangles;//Array of triangles in the Geometry. The BVH stores pointers to these triangles.
    BvhNode** backPtrs;//Array of BvhNodes. backPtrs[i] points to the node that triangles[i] resides.
    unsigned int maxTriangles, numTriangles;//Number of triangles we have space allocated for in the arrays, num triangles actually allocated in the array.
    Vec3 position;//position of the Geometry in 3D space
    unsigned long geometryID;//unique ID of this geometry
} Geometry;

Geometry genGeometry(unsigned int numTriangles, Vec3 position);
unsigned int geomAddTriangle(Geometry* geometry, Triangle triangle);

Triangle* geomGetTriangle(Geometry* geometry, unsigned int index);

//set the back pointer (address of Bvh node) to the triangle in the triangle array. This way by looking at a triangle we know which Bvh node it's in
//and as the Bvh node stores a pointer to the triangle in the array then by looking int the Bvh node we know what triangle we're looking at.
//this basically maps the triangle array to the BvhNode
unsigned int geomSetBackPtr(Geometry* geometry, unsigned int index, BvhNode* backPtr);


//geom ctr
unsigned long getGeomCtr(void);
void initGeomCtr(unsigned long ctrVal);

#endif