#include "../include/BvhNode.h"

BvhNode* bvhNodeGen(unsigned int childrenSize, unsigned int trianglesSize, BBox boundingBox)
{
  BvhNode* node = malloc(sizeof(BvhNode));
  if(node){
    node->numTriangles = 0;
    node->numChildren = 0;
    node->trianglesMax = trianglesSize;
    node->childrenMax = childrenSize;
    node->triangles = malloc(trianglesSize * sizeof(Triangle));
    if(node->triangles == NULL)
      return NULL;

    node->children = malloc(childrenSize * sizeof(BvhNode));
    if(node->children == NULL) {
      free(node->triangles);
      return NULL;
    }
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
	for(int i = 0; i < node->numChildren; i++)
	  bvhNodeFree(&node->children[i]);
      }

    }
    //We have no children below us so we can start cleaning ourselves up
    free(node->children);
    node->children == NULL;

    if(node->numTriangles > 0){
      if(node->triangles) {//cleanup the triangles
	for(int i = 0; i < node->numTriangles; i++)
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
    const unsigned int numToAllocate;//basically a default
    if(!node->children){//children array not allocated, we'll allocate it now
      node->children = malloc(numToAllocate * sizeof(BvhNode));
      node->childrenMax = numToAllocate;
      node->numChildren = 0;
    }

    //checking if we need to reallocate more space as it could be full
    if(node->numChildren >= node->childrenMax) {
      //make more space
      BvhNode* temp = malloc((node->numChildren + numToAllocate) * sizeof(BvhNode));
      //copy existing children to the new buffer
      memcpy(temp, node->children, node->numChildren * sizeof(BvhNode));
      //free the old buffer and point the children to the new buffer
      free(node->children);
      node->children = temp;
    }

    //we have space to put the child in the children list
    node->children[node->numChildren] = *child;
    node->numChildren++;
  }
}

void bvhAddTriangle(BvhNode* node, Triangle triangle)
{
  if(node){
    const unsigned int numToAllocate = 5;//basically just a default val
    if(!node->triangles){//check that the triangles list is allocated
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
      }
    //copy the triangle into the buffer
    node->triangles[node->numTriangles] = triangle;
    node->numTriangles++;
  }
}

int rayBoxIntersection(Ray ray, BBox* box, float* min, float* max)
{
  if(min && max){
    Vec3 invDir = {1.0f / ray.direction[0],
		   1.0f / ray.direction[1],
		   1.0f / ray.direction[2]
    };
    int sign[3] = {invDir[0] < 0, invDir[1] < 1, invDir[2] < 0};


    float txMin = (*box[0][0] - ray.origin[0]) * invDir[0];
    float txMax = (*box[1][0] - ray.origin[0]) * invDir[0];
    float tyMin = (*box[0][1] - ray.origin[1]) * invDir[1];
    float tyMax = (*box[1][1] - ray.origin[1]) * invDir[1];
    float tzMin = (*box[0][2] - ray.origin[2]) * invDir[2];
    float tzMax = (*box[1][2] - ray.origin[2]) * invDir[2];

    if((txMin > tyMax) || (tyMin > tzMax))
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

    *min = txMin;
    *max = txMax;
    return 1;
  }
  return 0;
}
