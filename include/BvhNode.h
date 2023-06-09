#ifndef __BVH_NODE_H__
#define __BVH_NODE_H__

#include "Maths.h"
#include "Ray.h"
#include "Triangle.h"
#include "Box.h"

typedef struct BvhNode {
  BBox boundingBox;//The bounding volume of this node
  struct BvhNode* children;//List of child BVhs
  Triangle* triangles;//List of triangles in this node
  unsigned int numChildren, numTriangles;//num triangles and nodes in the lists
  unsigned int childrenMax, trianglesMax;//max number (allocated space)
} BvhNode;

BvhNode* bvhNodeGen(unsigned int childrenSize, unsigned int trianglesSize, BBox boundingBox);
void bvhNodeFree(BvhNode* node);
void bvhAddChild(BvhNode* node, BvhNode* child);
void bvhAddTriangle(BvhNode* node, Triangle triangle);
int rayBoxIntersection(Ray ray, BBox* box, float* min, float* max);

#endif
