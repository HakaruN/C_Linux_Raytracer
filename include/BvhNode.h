#ifndef __BVH_NODE_H__
#define __BVH_NODE_H__

#include "Maths.h"
#include "Ray.h"
#include "Triangle.h"
#include "Box.h"

typedef struct BvhNode{
  BBox boundingBox;//The bounding volume of this node
  struct BvhNode** children;//Array of BVH node pointers  
  Triangle** triangles;//List of triangle pointers in this node
  unsigned int* geometryIds;//List of the geometry IDs that the triangles are part of. triangles[i] resides in geometryIDs[i]. This means if a Geometry want to modify a triangle we can check we're the owner of the tri
  unsigned int numChildren, numTriangles;//num triangles and nodes in the lists
  unsigned int childrenMax, trianglesMax;//max number (allocated space)
} BvhNode;

BvhNode* bvhNodeGen(unsigned int childrenSize, unsigned int trianglesSize, BBox boundingBox);
void bvhNodeFree(BvhNode* node);
void bvhAddChild(BvhNode* node, BvhNode* child);
BvhNode* bvhAddTriangle(BvhNode* node, unsigned int geomId, Triangle* triangle);
Triangle* testBVH(Ray* ray, BvhNode* node, Vec3 intersectionPoint);

#endif
