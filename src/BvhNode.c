#include "../include/BvhNode.h"

BvhNode* bvhNodeGen(unsigned int childrenSize, unsigned int geometriesSize, BBox* boundingBox)
{
  BvhNode* node = (BvhNode*)malloc(sizeof(BvhNode));
  if(node){
    memcpy(&node->boundingBox.min, boundingBox->min, sizeof(float) * 3);
    memcpy(&node->boundingBox.max, boundingBox->max, sizeof(float) * 3);
    node->numGeometries = 0;
    node->numChildren = 0;
    node->geometriesMax = geometriesSize;
    node->childrenMax = childrenSize;

    //allocate the space for the geoms
    node->geometries = malloc(geometriesSize * sizeof(Triangle));
    if(node->geometries == NULL)//if the geoms couldn't be allocated then deallocate the node and return null
    {
      free(node);
      return NULL;
    }

    //allocate the space for the children nodes
    node->children = malloc(childrenSize * sizeof(BvhNode));
    if(node->children == NULL) {//if the children couldn't be allocated then deallocate the node & geoms and return null
      free(node->geometries);
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

    if(node->numGeometries > 0){
      if(node->geometries) {//cleanup the geoms
	      for(unsigned int i = 0; i < node->numGeometries; i++)
	        GeometryFree(node->geometries[i]);
      }
    }
    //dealloc the geoms
    free(node->geometries);
    node->geometries = NULL;
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

/*
BvhNode* bvhAddGeometry(BvhNode* node, Geometry* geometry)
{
  if(node){
    if(node->numGeometries >= node->geometriesMax)
    {//if the Bvh is full, make a child that fits perfectly around the geometry and add it as a child to the parent. Then add the geom to that.
      if(node->numChildren == 0){
        BBox* b = geomGenAABB(geometry);
        BvhNode* childNode = bvhNodeGen(1, 1, b);
        bvhAddChild(node, childNode);
      }
      
      return bvhAddGeometry(node->children[0], geometry);
    }
    else
    {
      const unsigned int numToAllocate = 1;//basically just a default val
      if(!node->geometries){//check if the geom list is not allocated
        node->geometries = malloc(numToAllocate * sizeof(Geometry*));
        memset(node->geometries, (int)NULL, numToAllocate * sizeof(Geometry*));//NULL out the geoms ptrs
        node->geometriesMax = numToAllocate;
        node->numGeometries = 0;
      }
      if(node->numGeometries >= node->geometriesMax){
        //need to allocate more space
        Geometry** tmpGeoms = malloc((node->numGeometries + numToAllocate) * sizeof(Geometry*));
        memset(tmpGeoms, (int)NULL, (node->numGeometries + numToAllocate) * sizeof(Geometry*));//NULL out the whole temp ptr buffer
        //copy the existing geoms into the new buffer
        memcpy(tmpGeoms, node->geometries, node->numGeometries * sizeof(Geometry*));
        //free the old buffer and exchange the ptr
        free(node->geometries);
        node->geometries = tmpGeoms;
        node->geometriesMax = node->geometriesMax + numToAllocate;//Add space for the new geom(s)
      }

      //copy the geometry ptr into the buffer
      node->geometries[node->numGeometries] = geometry;
      node->numGeometries++;
      return node;
    }
  }
  return NULL;
}
*/

BvhNode* bvhAddG(BvhNode* node, G* geometry)
{
if(node){
    if(node->numGeometries >= node->geometriesMax)
    {//if the Bvh is full, make a child that fits perfectly around the geometry and add it as a child to the parent. Then add the geom to that.
      if(node->numChildren == 0){
        BBox* b = geomGenAABB(geometry);
        BvhNode* childNode = bvhNodeGen(1, 1, b);
        bvhAddChild(node, childNode);
      }
      
      return bvhAddG(node->children[0], geometry);
    }
    else
    {
      const unsigned int numToAllocate = 1;//basically just a default val
      if(!node->geometries){//check if the geom list is not allocated
        node->geometries = malloc(numToAllocate * sizeof(G*));
        memset(node->geometries, (int)NULL, numToAllocate * sizeof(G*));//NULL out the geoms ptrs
        node->geometriesMax = numToAllocate;
        node->numGeometries = 0;
      }
      if(node->numGeometries >= node->geometriesMax){
        //need to allocate more space
        G** tmpGeoms = malloc((node->numGeometries + numToAllocate) * sizeof(G*));
        memset(tmpGeoms, (int)NULL, (node->numGeometries + numToAllocate) * sizeof(G*));//NULL out the whole temp ptr buffer
        //copy the existing geoms into the new buffer
        memcpy(tmpGeoms, node->geometries, node->numGeometries * sizeof(G*));
        //free the old buffer and exchange the ptr
        free(node->geometries);
        node->geometries = tmpGeoms;
        node->geometriesMax = node->geometriesMax + numToAllocate;//Add space for the new geom(s)
      }

      //copy the geometry ptr into the buffer
      node->geometries[node->numGeometries] = geometry;
      node->numGeometries++;
      return node;
    }
  }
  return NULL;
}





/// @brief bottom up search
/// @param ray 
/// @param bvhNode 
/// @param intersectionPoint 
/// @param distance 
/// @return 
inline T* testBVH(Ray* ray, BvhNode* bvhNode, Vec3 intersectionPoint)
{
  T* closestTri = NULL;

  //recurse into our children to check for intersections
  int numChildren = bvhNode->numChildren;
  if(numChildren)
  {
    for(int i = 0; i < numChildren; i++)
    {
      T* triangle = testBVH(ray, bvhNode->children[i], intersectionPoint);
      if(triangle)
        closestTri = triangle;
    }
  }

  //check us for intersection
  int numGeometries = bvhNode->numGeometries;

  if(numGeometries)
  {
    //check if the ray intersects our bounding box
    if(rayBoxIntersection(ray, &bvhNode->boundingBox))
    {
      for(int g = 0; g < numGeometries; g++)
      {
        //Geometry* geometry = bvhNode->geometries[g];
        G* geometry = bvhNode->geometries[g];
        
        unsigned int numTris = geometry->numTriangles;
          for(int t = 0; t < numTris; t++)
          {
            T* triangle = &geometry->triangles[t];

#ifdef RELATIVE_VERTS 
            if(triangleIntersect(triangle->vertPosition[0], triangle->vertPosition[1], triangle->vertPosition[2], ray, intersectionPoint))
            //if(triangleIntersect(triangle->verts[0].transformedPosition, triangle->verts[1].transformedPosition, triangle->verts[2].transformedPosition, ray, intersectionPoint))
#else
           if(triangleIntersect(triangle->verts[0].position, triangle->verts[1].position, triangle->verts[2].position, ray, intersectionPoint))
#endif
        
            {

                closestTri = triangle;
            }
        }
      }
    }
  }

  //return the closest triangle we've found
  return closestTri;
}
