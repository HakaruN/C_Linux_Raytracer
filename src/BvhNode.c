#include "../include/BvhNode.h"

BvhNode* bvhNodeGen(unsigned int childrenSize, unsigned int trianglesSize, BBox boundingBox)
{
  BvhNode* node = malloc(sizeof(BvhNode));
  if(node){
    memcpy(&(node->boundingBox.min), &boundingBox.min, sizeof(float) * 3);
    memcpy(&(node->boundingBox.max), &boundingBox.max, sizeof(float) * 3);
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
    node->children = NULL;

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
    const unsigned int numToAllocate = 2;//basically a default
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

inline int rayBoxIntersection(Ray* ray, BBox* box, float* min, float* max)
{
  if(min && max){//is min and max a valid ptr
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

    *min = txMin;
    *max = txMax;
    return 1;
  }
  return 0;
}

Triangle* testBVH(Ray* ray, BvhNode* node, Vec3 intersectionPoint, float* distance)
{
  Triangle* tempClosestTri;
  float td = ray->distance;
  if(node->children && node->numChildren > 0){
    //Node has child nodes, we'll go into them until we reach the bottom of the tree
#ifdef DEBUG
    printf("Bvh node iterating through (%d) children\n", node->numChildren);
#endif
    for(unsigned int childIdx = 0; childIdx < node->numChildren; childIdx++)
    {
      Triangle* ttct = testBVH(ray, &node->children[childIdx], intersectionPoint, distance);
      if(td < ray->distance)
      {
        td = ray->distance;
        tempClosestTri = ttct;        
      }
    }
  }

  //see if we intersect the bounding box of the node
  Vec2 bounds;
  if(rayBoxIntersection(ray, &node->boundingBox, &bounds[0], &bounds[1])){
    //if we do intersect, go through the triangles of the node and test for intersections with them
    //    printf("Ray box intersection\n");
    if(node->triangles != NULL && node->numTriangles > 0){

      for(unsigned int triangleIdx = 0; triangleIdx < node->numTriangles; triangleIdx++){
	      Triangle* triangle = &node->triangles[triangleIdx];

        //Calculate normalised device coordinates
        Vec3 ndcVert0, ndcVert1, ndcVert2;
#ifdef RELATIVE_VERTS
        
        vec3Normalise(triangle->verts[0].transformedPosition, ndcVert0);
        vec3Normalise(triangle->verts[1].transformedPosition, ndcVert1);
        vec3Normalise(triangle->verts[2].transformedPosition, ndcVert2);
        //if(triangleIntersect(ndcVert0, ndcVert1, ndcVert2, ray, intersectionPoint)){
        
        if(triangleIntersect(triangle->verts[0].transformedPosition, triangle->verts[1].transformedPosition,
                triangle->verts[2].transformedPosition, ray, intersectionPoint)){
#else
	      if(triangleIntersect(triangle->verts[0].position, triangle->verts[1].position, triangle->verts[2].position, ray, intersectionPoint)){
#endif

          //if we intersect with the triangle, check if its the closes triangle we've hit so far. If so mark it.
          if(ray->distance < *distance){//if its closer, mark it
            //printf("Curent min distance %f\n", *distance);
            //printf("intersection distance %f\n", ray->distance);
            //tempClosestTri = &node->triangles[triangleIdx];
            //return tempClosestTri;
            return &node->triangles[triangleIdx];
	        }
	      }
      }      
    }

  }
  return NULL;
}
