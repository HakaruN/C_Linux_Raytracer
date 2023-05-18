#include "../include/Maths.h"


inline void cross(Vec3 a, Vec3 b, Vec3 result)
{
  result[0] = a[1] * b[2] - a[2] * b[1];
  result[1] = a[2] * b[0] - a[0] * b[2];
  result[2] = a[0] * b[1] - a[1] * b[0];
}

inline float dot(Vec3 a, Vec3 b)
{
  return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

inline void vec3Add(Vec3 a, Vec3 b, Vec3 result)
{
#ifdef ALTIVEC
  result vec_add(a,b);
#else
  if(a && b && result){
    result[0] = a[0] + b[0];
    result[1] = a[1] + b[1];
    result[2] = a[2] + b[2];
  }
#endif
}

inline void vec3ScalarMult(Vec3 vec, float scalar, Vec3 result)
{
  if(vec && result)
    {
      result[0] = vec[0] * scalar;
      result[1] = vec[1] * scalar;
      result[2] = vec[2] * scalar;
    }
}

inline float vec3Displacement(Vec3 a, Vec3 b)
{
  if(a && b)
    {
      //      #define AV

#ifdef ALTIVEC
      vector float delta;
      vector float va = {a[0],a[1],a[2],0};
      vector float vb = {b[0],b[1],b[2],0};
      delta = vec_sub(va,vb);
      float dx = delta[0]*delta[0];
      float dy = delta[1]*delta[1];
      float dz = delta[2]*delta[2];
      float cum = dx + dy + dz;
      return sqrt(cum);
#else

      float dx, dy, dz;
      dx = a[0] - b[0];
      dy = a[1] - b[1];
      dz = a[2] - b[2];
      return sqrt((dx*dx) + (dy*dy) + (dz*dz));

#endif
    }
  return 0;
}


inline float normalise(float val, float fromMin, float fromMax, float toMin, float toMax)
{
  return toMin + (val - fromMin)/(fromMax - fromMin) * (toMax - toMin);
}

inline void printVec2(Vec2 a)
{
  printf("Vec2: %f, %f\n", a[0], a[1]);
}

void printVec3(Vec3 a)
{
  printf("vec3: %f, %f, %f\n", a[0], a[1], a[2]);

}
