#ifndef __MATHS_H__
#define __MATHS_H__


#include <stdio.h>
#include <math.h>

#ifdef ALTIVEC
#include <altivec.h>
#endif


typedef float Vec1;
typedef float Vec2[2];
typedef float Vec3[3];
typedef float Vec4[4];


void cross(Vec3 a, Vec3 b, Vec3 result);
float dot(Vec3 a, Vec3 b);
void vec3Add(Vec3 a, Vec3 b, Vec3 result);
void vec3ScalarMult(Vec3 vec, float scalar, Vec3 result);
float vec3Displacement(Vec3 a, Vec3 b);
float normalise(float val, float fromMin, float fromMax, float toMin, float toMax);
void printVec3(Vec3 a);


#endif
