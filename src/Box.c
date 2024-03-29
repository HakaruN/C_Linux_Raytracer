#include "../include/Box.h"


inline BBox* genBox(Vec3 min, Vec3 max)
{
  BBox* b = (BBox*)malloc(sizeof(BBox));
  #ifdef DEBUG
  printf("Generating box\n");
  printVec3(min);
  printVec3(max);
#endif
  memcpy(b->min, min, 3 * sizeof(float));
  memcpy(b->max, max, 3 * sizeof(float));
  return b;
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