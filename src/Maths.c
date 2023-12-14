#include "../include/Maths.h"


inline float degToRad(float deg)
{
  return deg * 0.0174532925;
}
inline float radToDeg(float rad)
{
  return rad * 57.2957795;
}

inline void cross(Vec3 a, Vec3 b, Vec3 result)
{
  if(a && b && result){
  result[0] = a[1] * b[2] - a[2] * b[1];
  result[1] = a[2] * b[0] - a[0] * b[2];
  result[2] = a[0] * b[1] - a[1] * b[0];
  }
}

inline float vec3Len(Vec3 a)
{
  if(a){
  return sqrt((a[0] * a[0]) + (a[1] * a[1]) + (a[2] * a[2]));
  }
  return 0;
}

inline float vec4Len(Vec4 a)
{
  if(a){
  return sqrt((a[0] * a[0]) + (a[1] * a[1]) + (a[2] * a[2]) + (a[3] * a[3]));
  }
  return 0;
}


inline float vec3Dot(Vec3 a, Vec3 b)
{
  if(a && b){
  return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
  }
  return 0;
}

inline float vec4Dot(Vec4 a, Vec4 b)
{
  if(a && b){
  return a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + a[3] * b[3];
  }
  return 0;
}

/*
inline void vec3OrthogProjec(Vec3 a, Vec3 b, Vec3 res)
{
  if(a && b && res){
  float bDot = vec3Dot(a, b);
  float aMag = vec3Len(a);
  vec3ScalarMult(a, bDot, res);
  }
}

inline void vec4OrthogProjec(Vec4 a, Vec4 b, Vec4 res)
{
  if(a && b && res){
  float bDot = vec4Dot(a, b);
  vec4ScalarMult(a, bDot, res);
  }
}
*/

inline void vec3Normalise(Vec3 vec, Vec3 result)
{
  if(vec && result){
  float magnitude = vec3Len(vec);
  //do 1 division (inv magnitude) then I can do 3 mults with the invMag and not have to do 3 divs with the mag.
  //It makes it a tad faster
  float invMag = 1/magnitude;
  result[0] = vec[0] * invMag;
  result[1] = vec[1] * invMag;
  result[2] = vec[2] * invMag;
  }
}

inline void vec4Normalise(Vec4 vec, Vec4 result)
{
  if(vec && result){
  float magnitude = vec4Len(vec);
  //do 1 division (inv magnitude) then I can do 3 mults with the invMag and not have to do 3 divs with the mag.
  //It makes it a tad faster
  float invMag = 1/magnitude;
  result[0] = vec[0] * invMag;
  result[1] = vec[1] * invMag;
  result[2] = vec[2] * invMag;
  result[3] = vec[3] * invMag;
  }
}

inline void vec3Add(Vec3 a, Vec3 b, Vec3 result)
{
  #ifdef ALTIVEC
  if(a && b && result){
  vector float temp = vec_add(
			      (vector float){a[0],a[1],a[2],0},
			      (vector float){b[0],b[1],b[2],0});
  result[0] = temp[0];
  result[1] = temp[1];
  result[2] = temp[2];
  }
#else
  if(a && b && result){
    result[0] = a[0] + b[0];
    result[1] = a[1] + b[1];
    result[2] = a[2] + b[2];
    }
  #endif
}

inline void vec4Add(Vec4 a, Vec4 b, Vec4 result)
{
  #ifdef ALTIVEC
  if(a && b && result){
  vector float temp = vec_add(
			      (vector float){a[0],a[1],a[2],a[3]},
			      (vector float){b[0],b[1],b[2],b[3]});
  result[0] = temp[0];
  result[1] = temp[1];
  result[2] = temp[2];
  result[3] = temp[3];
  }
#else
  if(a && b && result){
    result[0] = a[0] + b[0];
    result[1] = a[1] + b[1];
    result[2] = a[2] + b[2];
    result[3] = a[3] + b[3];
    }
  #endif
}

inline void vec3Sub(Vec3 a, Vec3 b, Vec3 result)
{
#ifdef ALTIVEC
  if(a && b && result){
  vector float temp = vec_sub(
			      (vector float){a[0],a[1],a[2],0},
			      (vector float){b[0],b[1],b[2],0});
  result[0] = temp[0];
  result[1] = temp[1];
  result[2] = temp[2];
  }
#else
  if(a && b && result){
    result[0] = a[0] - b[0];
    result[1] = a[1] - b[1];
    result[2] = a[2] - b[2];
  }
#endif
}

inline void vec4Sub(Vec4 a, Vec4 b, Vec4 result)
{
#ifdef ALTIVEC
  if(a && b && result){
  vector float temp = vec_sub(
			      (vector float){a[0],a[1],a[2],a[3]},
			      (vector float){b[0],b[1],b[2],b[3]});
  result[0] = temp[0];
  result[1] = temp[1];
  result[2] = temp[2];
  result[3] = temp[3];
  }
#else
  if(a && b && result){
    result[0] = a[0] - b[0];
    result[1] = a[1] - b[1];
    result[2] = a[2] - b[2];
    result[3] = a[3] - b[3];
  }
#endif
}

inline void vec3PairwiseMult(Vec3 a, Vec3 b, Vec3 result)
{
  if(a && b)
    {
      result[0] = a[0] * b[0];
      result[1] = a[1] * b[1];
      result[2] = a[2] * b[2];
    }
}

inline void vec4PairwiseMult(Vec4 a, Vec4 b, Vec4 result)
{
  if(a && b)
    {
      result[0] = a[0] * b[0];
      result[1] = a[1] * b[1];
      result[2] = a[2] * b[2];
      result[3] = a[3] * b[3];
    }
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

inline void vec4ScalarMult(Vec4 vec, float scalar, Vec4 result)
{
  if(vec && result)
    {
      result[0] = vec[0] * scalar;
      result[1] = vec[1] * scalar;
      result[2] = vec[2] * scalar;
      result[4] = vec[4] * scalar;
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

/*
inline float angleBetweenVec3(Vec3 a, Vec3 b)
{
  float aLen = vec3Len(a);
  float bLen = vec3Len(b);

}

inline float angleBetweenVec4(Vec3 a, Vec3 b)
{

}
*/


//Matrix operation
inline void mat3Mul(Mat3 a, Mat3 b, Mat3 c)
{
  //Max[ROW][COL]
  #ifdef ALTIVEC
  #else
  //calc top row c
  c[0][0] = (a[0][0] * b[0][0]) + (a[0][1] * b[1][0]) + (a[0][2] * b[2][0]);//first row first col
  c[0][1] = (a[0][0] * b[0][1]) + (a[0][1] * b[1][1]) + (a[0][2] * b[2][1]);//first row secnd col
  c[0][2] = (a[0][0] * b[0][2]) + (a[0][1] * b[1][2]) + (a[0][2] * b[2][2]);//first row third col
  //calc middle row c
  c[1][0] = (a[1][0] * b[0][0]) + (a[1][1] * b[1][0]) + (a[1][2] * b[2][0]);//sec row first col
  c[1][1] = (a[1][0] * b[0][1]) + (a[1][1] * b[1][1]) + (a[1][2] * b[2][1]);//sec row secnd col
  c[1][2] = (a[1][0] * b[0][2]) + (a[1][1] * b[1][2]) + (a[1][2] * b[2][2]);//sec row third col
  //calc bottom row c
  c[2][0] = (a[2][0] * b[0][0]) + (a[2][1] * b[1][0]) + (a[2][2] * b[2][0]);//thrd row frst col
  c[2][1] = (a[2][0] * b[0][1]) + (a[2][1] * b[1][1]) + (a[2][2] * b[2][1]);//thrd row scnd col
  c[2][2] = (a[2][0] * b[0][2]) + (a[2][1] * b[1][2]) + (a[2][2] * b[2][2]);//thrd row thrd col
  #endif
}

//Matrix operation
inline void mat4Mul(Mat4 a, Mat4 b, Mat4 c)
{
  //Max[ROW][COL]
  #ifdef ALTIVEC

  #else
  //calc first row c
  c[0][0] = (a[0][0] * b[0][0]) + (a[0][1] * b[1][0]) + (a[0][2] * b[2][0]) + (a[0][3] * b[3][0]);//first row first col
  c[0][1] = (a[0][0] * b[0][1]) + (a[0][1] * b[1][1]) + (a[0][2] * b[2][1]) + (a[0][3] * b[3][1]);//first row secnd col
  c[0][2] = (a[0][0] * b[0][2]) + (a[0][1] * b[1][2]) + (a[0][2] * b[2][2]) + (a[0][3] * b[3][2]);//first row third col
  c[0][3] = (a[0][0] * b[0][3]) + (a[0][1] * b[1][3]) + (a[0][2] * b[2][3]) + (a[0][3] * b[3][3]);//first row forth col
  //calc second row c
  c[1][0] = (a[1][0] * b[0][0]) + (a[1][1] * b[1][0]) + (a[1][2] * b[2][0]) + (a[1][3] * b[3][0]);//secnd row first col
  c[1][1] = (a[1][0] * b[0][1]) + (a[1][1] * b[1][1]) + (a[1][2] * b[2][1]) + (a[1][3] * b[3][1]);//secnd row secnd col
  c[1][2] = (a[1][0] * b[0][2]) + (a[1][1] * b[1][2]) + (a[1][2] * b[2][2]) + (a[1][3] * b[3][2]);//secnd row third col
  c[1][3] = (a[1][0] * b[0][3]) + (a[1][1] * b[1][3]) + (a[1][2] * b[2][3]) + (a[1][3] * b[3][3]);//secnd row forth col
  //calc third row c
  c[2][0] = (a[2][0] * b[0][0]) + (a[2][1] * b[1][0]) + (a[2][2] * b[2][0]) + (a[2][3] * b[3][0]);//third row frst col
  c[2][1] = (a[2][0] * b[0][1]) + (a[2][1] * b[1][1]) + (a[2][2] * b[2][1]) + (a[2][3] * b[3][1]);//third row scnd col
  c[2][2] = (a[2][0] * b[0][2]) + (a[2][1] * b[1][2]) + (a[2][2] * b[2][2]) + (a[2][3] * b[3][2]);//third row thrd col
  c[2][3] = (a[2][0] * b[0][3]) + (a[2][1] * b[1][3]) + (a[2][2] * b[2][3]) + (a[2][3] * b[3][3]);//third row frth col
  //calc fourth row c
  c[3][0] = (a[3][0] * b[0][0]) + (a[3][1] * b[1][0]) + (a[3][2] * b[2][0]) + (a[3][3] * b[3][0]);//fourth row frst col
  c[3][1] = (a[3][0] * b[0][1]) + (a[3][1] * b[1][1]) + (a[3][2] * b[2][1]) + (a[3][3] * b[3][1]);//fourth row scnd col
  c[3][2] = (a[3][0] * b[0][2]) + (a[3][1] * b[1][2]) + (a[3][2] * b[2][2]) + (a[3][3] * b[3][2]);//fourth row thrd col
  c[3][3] = (a[3][0] * b[0][3]) + (a[3][1] * b[1][3]) + (a[3][2] * b[2][3]) + (a[3][3] * b[3][3]);//fourth row frth col
  #endif
}


inline void mat3VecMul(Mat3 a, Vec3 b, Vec3 res)
{
  res[0] = (a[0][0] * b[0]) + (a[0][1] * b[1]) + (a[0][2] * b[2]);//first row first col
  res[1] = (a[1][0] * b[0]) + (a[1][1] * b[1]) + (a[1][2] * b[2]);//secnd row first col
  res[2] = (a[2][0] * b[0]) + (a[2][1] * b[1]) + (a[2][2] * b[2]);//third row frst col
}

inline void mat4VecMul(Mat4 a, Vec4 b, Vec4 res)
{
  res[0] = (a[0][0] * b[0]) + (a[0][1] * b[1]) + (a[0][2] * b[2]) + (a[0][3] * b[3]);//first row first col
  res[1] = (a[1][0] * b[0]) + (a[1][1] * b[1]) + (a[1][2] * b[2]) + (a[1][3] * b[3]);//secnd row first col
  res[2] = (a[2][0] * b[0]) + (a[2][1] * b[1]) + (a[2][2] * b[2]) + (a[2][3] * b[3]);//third row frst col
  res[3] = (a[3][0] * b[0]) + (a[3][1] * b[1]) + (a[3][2] * b[2]) + (a[3][3] * b[3]);//third row frst col
}

void mat3GetUnitMat(Mat3 res)
{
  res[0][0] = 1; res[0][1] = 0; res[0][2] = 0; 
  res[1][0] = 0; res[1][1] = 1; res[1][2] = 0; 
  res[2][0] = 0; res[2][1] = 0; res[2][2] = 1; 
}

void mat4GetUnitMat(Mat4 res)
{  
  res[0][0] = 1; res[0][1] = 0; res[0][2] = 0; res[0][3] = 0;
  res[1][0] = 0; res[1][1] = 1; res[1][2] = 0; res[1][3] = 0;
  res[2][0] = 0; res[2][1] = 0; res[2][2] = 1; res[2][3] = 0;
  res[3][0] = 0; res[3][1] = 0; res[3][2] = 0; res[3][3] = 1;
}

void mat4GetTranslateMat(Vec3 t, Mat4 res)
{
  res[0][0] = 1; res[0][1] = 0; res[0][2] = 0; res[0][3] = t[0];
  res[1][0] = 0; res[1][1] = 1; res[1][2] = 0; res[1][3] = t[1];
  res[2][0] = 0; res[2][1] = 0; res[2][2] = 1; res[2][3] = t[2];
  res[3][0] = 0; res[3][1] = 0; res[3][2] = 0; res[3][3] = 1;
}

void mat4GetScaleMat(Vec3 t, Mat4 res)
{
  res[0][0] = t[0]; res[0][1] = 0;    res[0][2] = 0;    res[0][3] = 0;
  res[1][0] = 0;    res[1][1] = t[1]; res[1][2] = 0;    res[1][3] = 0;
  res[2][0] = 0;    res[2][1] = 0;    res[2][2] = t[2]; res[2][3] = 0;
  res[3][0] = 0;    res[3][1] = 0;    res[3][2] = 0;    res[3][3] = 1;
}

void mat4GetRotateXMat(float angle, Mat4 res)
{
  res[0][0] = 0;          res[0][1] = 0;                        res[0][2] = 0;                        res[0][3] = 0;
  res[1][0] = 0;          res[1][1] = cosf(degToRad(angle));    res[1][2] = -sinf(degToRad(angle));   res[1][3] = 0;
  res[2][0] = 0;          res[2][1] = sinf(degToRad(angle));    res[2][2] = cosf(degToRad(angle));    res[2][3] = 0;
  res[3][0] = 0;          res[3][1] = 0;                        res[3][2] = 0;                        res[3][3] = 1;  
}

void mat4GetRotateYMat(float angle, Mat4 res)
{
  res[0][0] = cosf(degToRad(angle));      res[0][1] = 0;    res[0][2] = sinf(degToRad(angle));    res[0][3] = 0;
  res[1][0] = 0;                          res[1][1] = 1;    res[1][2] = 0;                        res[1][3] = 0;
  res[2][0] = -sinf(degToRad(angle));     res[2][1] = 0;    res[2][2] = cosf(degToRad(angle));    res[2][3] = 0;
  res[3][0] = 0;                          res[3][1] = 0;    res[3][2] = 0;                        res[3][3] = 1;
}

void mat4GetRotateZMat(float angle, Mat4 res)
{
  res[0][0] = cosf(degToRad(angle));      res[0][1] = -sinf(degToRad(angle));   res[0][2] = 0;    res[0][3] = 0;
  res[1][0] = sinf(degToRad(angle));      res[1][1] = cosf(degToRad(angle));    res[1][2] = 0;    res[1][3] = 0;
  res[2][0] = 0;                          res[2][1] = 0;                        res[2][2] = 1;    res[2][3] = 0;
  res[3][0] = 0;                          res[3][1] = 0;                        res[3][2] = 0;    res[3][3] = 1;
}

inline float normalise(float val, float fromMin, float fromMax, float toMin, float toMax)
{
  return toMin + (val - fromMin)/(fromMax - fromMin) * (toMax - toMin);
}

inline void printVec2(Vec2 a)
{
  printf("Vec2: %f, %f\n", a[0], a[1]);
}

inline void printVec3(Vec3 a)
{
  printf("vec3: %f, %f, %f\n", a[0], a[1], a[2]);

}

inline void printVec4(Vec4 a)
{
  printf("vec3: %f, %f, %f, %f\n", a[0], a[1], a[2], a[3]);

}


inline void printMat3(Mat3 a)
{  
  printf("Mat3: \n");
  printf("______\n");
  printf("|%.3g %.3g %.3g|\n", a[0][0], a[0][1], a[0][2]);
  printf("|%.3g %.3g %.3g|\n", a[1][0], a[1][1], a[1][2]);
  printf("|%.3g %.3g %.3g|\n", a[2][0], a[2][1], a[2][2]);
  printf("______\n");
}

inline void printMat4(Mat4 a)
{
  printf("Mat4: \n");
  printf("________\n");
  printf("|%.3g %.3g %.3g %.3g|\n", a[0][0], a[0][1], a[0][2], a[0][3]);
  printf("|%.3g %.3g %.3g %.3g|\n", a[1][0], a[1][1], a[1][2], a[1][3]);
  printf("|%.3g %.3g %.3g %.3g|\n", a[2][0], a[2][1], a[2][2], a[2][3]);
  printf("|%.3g %.3g %.3g %.3g|\n", a[3][0], a[3][1], a[3][2], a[3][3]);
  printf("________\n");
  printf("\n");
}
