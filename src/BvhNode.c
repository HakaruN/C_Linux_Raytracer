#include "../include/BvhNode.h"

BvhNode* bvhNodeGen(unsigned int childrenSize, unsigned int trianglesSize, BBox boundingBox)
{
  BvhNode* node = (BvhNode*)malloc(sizeof(BvhNode));
  if(node){
    memcpy(&(node->boundingBox.min), &boundingBox.min, sizeof(float) * 3);
    memcpy(&(node->boundingBox.max), &boundingBox.max, sizeof(float) * 3);
    node->numTriangles = 0;
    node->numChildren = 0;
    node->trianglesMax = trianglesSize;
    node->childrenMax = childrenSize;

    //allocate the space for the triangles
    node->triangles = malloc(trianglesSize * sizeof(Triangle));
    if(node->triangles == NULL)//if the triangles couldn't be allocated then deallocate the node and return null
    {
      free(node);
      return NULL;
    }

    //allocate the space for the children nodes
    node->children = malloc(childrenSize * sizeof(BvhNode));
    if(node->children == NULL) {//if the children couldn't be allocated then deallocate the node & tris and return null
      free(node->triangles);
      free(node);
      return NULL;
    }

    //init the children array to NULL
    for(int i = 0; i < childrenSize; i++)
      node->children[i] = NULL;

    return node;
  }
  return NULL;
}

void bvhNodeFree(BvhNode* node)
{
  if(node) {
    //If the node has children, we will recurse through them calling free on the children.
    //This means we go through to the bottom of the tree and free on the way up.
    //This stops nodes getting orphaned.
    if(node->numChildren > 0) {
      if(node->children) {
	      for(int i = 0; i < node->numChildren; i++){
          if(node->children[i])//the child ptr is actually pointing at something
	          bvhNodeFree(node->children[i]);
        }
      }
    }

    //We have no children below us so we can start cleaning ourselves up
    free(node->children);
    node->children = NULL;

    if(node->numTriangles > 0){
      if(node->triangles) {//cleanup the triangles
	      for(unsigned int i = 0; i < node->numTriangles; i++)
	        freeTriangle(&node->triangles[i]);
      }
    }
    //dealloc the triangles
    free(node->triangles);
    node->triangles = NULL;
  }
}


void bvhAddChild(BvhNode* node, BvhNode* child)
{
  if(child && node){
    const unsigned int numToAllocate = 2;//basically a default
    if(!node->children){//children array not allocated, we'll allocate it now
      node->children = malloc(numToAllocate * sizeof(BvhNode));
      node->childrenMax = numToAllocate;
      node->numChildren = 0;
    }

    //checking if we need to reallocate more space as it could be full
    if(node->numChildren >= node->childrenMax) {
      //make more space
      BvhNode** temp = malloc((node->numChildren + numToAllocate) * sizeof(BvhNode));
      for(int i = 0; i < node->numChildren + numToAllocate; i++)
        temp[i] = NULL;

      //copy existing children to the new buffer
      memcpy(temp, node->children, node->numChildren * sizeof(BvhNode));
      //free the old buffer and point the children to the new buffer
      free(node->children);
      node->children = temp;
      node->childrenMax = node->childrenMax + numToAllocate;//Add space for the new children
    }

    //we have space to put the child in the children list
    node->children[node->numChildren] = child;
    node->numChildren++;
  }
}

void bvhAddTriangle(BvhNode* node, Triangle triangle)
{
  if(node){
    const unsigned int numToAllocate = 2;//basically just a default val
    if(!node->triangles){//check if the triangles list is not allocated
      node->triangles = malloc(numToAllocate * sizeof(Triangle));
      node->trianglesMax = numToAllocate;
      node->numTriangles = 0;
    }
    if(node->numTriangles >= node->trianglesMax){
      //need to allocate more space
      Triangle* temp = malloc((node->numTriangles + numToAllocate) * sizeof(Triangle));
      //copy the existing triangles into the new buffer
      memcpy(temp, node->triangles, node->numTriangles * sizeof(Triangle));
      //free the old buffer and exchange the ptr
      free(node->triangles);
      node->triangles = temp;
      node->trianglesMax = node->trianglesMax + numToAllocate;//Add space for the new triangle(s)
    }
    //copy the triangle into the buffer
    node->triangles[node->numTriangles] = triangle;
    node->numTriangles++;
  }
}

inline int rayBoxIntersection(Ray* ray, BBox* box)
{
    Vec3 invDir = {
      1.0f / ray->direction[0],
		  1.0f / ray->direction[1],
		  1.0f / ray->direction[2]
    };
    //    printf("Min: ");
    //    printVec3(box->min);
    //    printf("Max: ");
    //    printVec3(box->max);

    float txMin = (box->min[0] - ray->origin[0]) * invDir[0];
    float txMax = (box->max[0] - ray->origin[0]) * invDir[0];
    float tyMin = (box->min[1] - ray->origin[1]) * invDir[1];
    float tyMax = (box->max[1] - ray->origin[1]) * invDir[1];
    float tzMin = (box->min[2] - ray->origin[2]) * invDir[2];
    float tzMax = (box->max[2] - ray->origin[2]) * invDir[2];

    if((txMin > tyMax) || (tyMin > txMax))
      return 0;

    //txMin and txMax double up as temp vals during the algorithm
    if(tyMin > txMin)
      txMin = tyMin;

    if(tyMax < txMax)
      txMax = tyMax;

    if((txMin > tzMax) || (tzMin > txMax))
      return 0;

    if(tzMin > txMin)
      txMin = tzMin;

    if(tzMax < txMax)
      txMax = tzMax;
      
    return 1;
}



/// @brief bottom up search
/// @param ray 
/// @param bvhNode 
/// @param intersectionPoint 
/// @param distance 
/// @return 
Triangle* testBVH(Ray* ray, BvhNode* bvhNode, Vec3 intersectionPoint, float* distance)
{
  Triangle* closestTri = NULL;

  //recurse into our children to check for intersections
  int numChildren = bvhNode->numChildren;
  if(numChildren)
  {
    for(int i = 0; i < numChildren; i++)
    {
      Triangle* triangle = testBVH(ray, bvhNode->children[i], intersectionPoint, distance);
      if(triangle)
        closestTri = triangle;
    }
  }

  //check us for intersection
  int numTriangles = bvhNode->numTriangles;

  if(numTriangles)
  {
    //check if the ray intersects our bounding box
    if(rayBoxIntersection(ray, &bvhNode->boundingBox))
    {
      for(int k = 0; k < numTriangles; k++)
      {
        Triangle* triangle = &(bvhNode->triangles[k]);
#ifdef RELATIVE_VERTS
        if(triangleIntersect(triangle->verts[0].transformedPosition, triangle->verts[1].transformedPosition, triangle->verts[2].transformedPosition, ray, intersectionPoint))
#else
        if(triangleIntersect(triangle->verts[0].position, triangle->verts[1].position, triangle->verts[2].position, ray, intersectionPoint))
#endif
        {

            closestTri = triangle;
        }
      }
    }
  }

  //return the closest triangle we've found
  return closestTri;
}
