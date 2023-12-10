#ifndef __BVH_NODE_H__
#define __BVH_NODE_H__

#include "Maths.h"
#include "Ray.h"
#include "Box.h"
#include "Geometry.h"


static const unsigned int bvhNumChildrenMax = 2;

typedef struct BvhNode{
  BBox boundingBox;//The bounding volume of this node
  struct BvhNode** children;//Array of BVH node pointers  
  G** geometries;//List of geometry pointers in this node
  unsigned int numChildren, numGeometries;//num geometrys and nodes in the lists
  unsigned int childrenMax, geometriesMax;//max number (allocated space)
} BvhNode;

BvhNode* bvhNodeGen(unsigned int childrenSize, unsigned int geometriesSize, BBox* boundingBox);
void bvhNodeFree(BvhNode* node);
void bvhAddChild(BvhNode* node, BvhNode* child);
//BvhNode* bvhAddGeometry(BvhNode* node, Geometry* triangle);
BvhNode* bvhAddG(BvhNode* node, G* triangle);
T* testBVH(Ray* ray, BvhNode* node, Vec3 intersectionPoint);


#endif
