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
      #ifdef FX
      sFXDiv(ItoFX(1), ray->direction[0]),
		  sFXDiv(ItoFX(1), ray->direction[1]),
		  sFXDiv(ItoFX(1), ray->direction[2]),
      #else
      1.0f / ray->direction[0],
		  1.0f / ray->direction[1],
		  1.0f / ray->direction[2]
      #endif
    };
    //    printf("Min: ");
    //    printVec3(box->min);
    //    printf("Max: ");
    //    printVec3(box->max);
    #define OK 0
    #define POS_INF 1
    #define POS_NAN 2
    #define NEG_INF 4
    #define NEG_NAN 8

    //These handle floating point edge cases for the fixed point implementation
    int txMinState = OK, txMaxState = OK;
    int tyMinState = OK, tyMaxState = OK;
    int tzMinState = OK, tzMaxState = OK;

    #ifdef FX

    SFX16_16 txMin = sFXMul((box->min[0] - ray->origin[0]), invDir[0]);
    SFX16_16 txMax = sFXMul((box->max[0] - ray->origin[0]), invDir[0]);    
    if(invDir[0] == 0b01111111111111111111111111111111) {
      txMinState = NEG_NAN; txMaxState = POS_INF;
      }
    
    
    SFX16_16 tyMin = sFXMul((box->min[1] - ray->origin[1]), invDir[1]);
    SFX16_16 tyMax = sFXMul((box->max[1] - ray->origin[1]), invDir[1]);
    if(invDir[1] == 0b01111111111111111111111111111111) {
      tyMinState = NEG_NAN; tyMaxState = POS_INF;
      }
     
    
    SFX16_16 tzMin = sFXMul((box->min[2] - ray->origin[2]), invDir[2]);
    SFX16_16 tzMax = sFXMul((box->max[2] - ray->origin[2]), invDir[2]);
    if(invDir[2] == 0b01111111111111111111111111111111) {
      tzMinState = NEG_NAN; tzMaxState = POS_INF;
      }


    #else
    float txMin = (box->min[0] - ray->origin[0]) * invDir[0];
    float txMax = (box->max[0] - ray->origin[0]) * invDir[0];
    float tyMin = (box->min[1] - ray->origin[1]) * invDir[1];
    float tyMax = (box->max[1] - ray->origin[1]) * invDir[1];
    float tzMin = (box->min[2] - ray->origin[2]) * invDir[2];
    float tzMax = (box->max[2] - ray->origin[2]) * invDir[2];
    #endif
      
      //check not only the values but their state (the numerical value could be invalid as the state is set)
    if(
      (txMin > tyMax) && (txMinState != NEG_INF && txMinState != NEG_NAN) && (tyMaxState != POS_INF && tyMaxState != POS_NAN) || 
      (tyMin > txMax) && (tyMinState != NEG_INF && tyMinState != NEG_NAN) && (txMaxState != POS_INF && tyMaxState != POS_NAN)
      )
        return 0;

    //txMin and txMax double up as temp vals during the algorithm
    //  -nan      -nan
    if(tyMin > txMin && ((tyMinState != NEG_INF && tyMinState != NEG_NAN) && (txMinState != POS_INF && txMinState != POS_NAN))) {
      txMin = tyMin;
    }
     // +inf   +inf      
    if(tyMax < txMax && ((tyMaxState != POS_INF && tyMaxState != POS_NAN) && (txMaxState != NEG_INF && txMaxState != NEG_NAN))) {
      txMax = tyMax;
    }
      // -nan   110        10       -nan
    if(
      (txMin > tzMax) && (txMinState != NEG_INF && txMinState != NEG_NAN) && (tyMaxState != POS_INF && tyMaxState != POS_NAN) ||
      (tzMin > txMax) && (tyMinState != NEG_INF && tyMinState != NEG_NAN) && (txMaxState != POS_INF && tyMaxState != POS_NAN))
      return 0;
       //10    -nan
    if(tzMin > txMin)
      txMin = tzMin;

      //110      +inf
    if(tzMax < txMax)
      txMax = tzMax;
      
    return 1;
}