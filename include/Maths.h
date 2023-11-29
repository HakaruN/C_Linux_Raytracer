#ifndef __MATHS_H__
#define __MATHS_H__

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>


#ifdef ALTIVEC
#include <altivec.h>
#endif

#include "../include/FP.h"

#ifdef FX
typedef SFX16_16 Vec1;
typedef SFX16_16 Vec2[2];
typedef SFX16_16 Vec3[3];
typedef SFX16_16 Vec4[4];
#else
typedef float Vec1;
typedef float Vec2[2];
typedef float Vec3[3];
typedef float Vec4[4];
#endif

typedef Vec2 Mat2[2];
typedef Vec3 Mat3[3];
typedef Vec4 Mat4[4];

#ifdef FX
 SFX16_16 degToRad(SFX16_16 deg);
#else
 float degToRad(float deg);
#endif

#ifdef FX
 SFX16_16 radToDeg(SFX16_16 rad);
#else
 float radToDeg(float rad);
#endif

void cross(Vec3 a, Vec3 b, Vec3 result);
#ifdef FX
 SFX16_16 dot(Vec3 a, Vec3 b);
#else
 float dot(Vec3 a, Vec3 b);
#endif

void vec3Normalise(Vec3 vec, Vec3 result);
void vec3Add(Vec3 a, Vec3 b, Vec3 result);
void vec3Sub(Vec3 a, Vec3 b, Vec3 result);
void vec3PairwiseMult(Vec3 a, Vec3 b, Vec3 result);
#ifdef FX
 void vec3ScalarMult(Vec3 vec, SFX16_16 scalar, Vec3 result);
#else
 void vec3ScalarMult(Vec3 vec, float scalar, Vec3 result);
#endif
#ifdef FX
 SFX16_16 vec3Displacement(Vec3 a, Vec3 b);
#else
 float vec3Displacement(Vec3 a, Vec3 b);
#endif

void mat3Mul(Mat3 a, Mat3 b, Mat3 res);
void mat4Mul(Mat4 a, Mat4 b, Mat4 res);

#ifdef FX
 SFX16_16 normalise(SFX16_16 val, SFX16_16 fromMin, SFX16_16 fromMax, SFX16_16 toMin, SFX16_16 toMax);
#else
 float normalise(float val, float fromMin, float fromMax, float toMin, float toMax);
#endif

void printVec2(Vec2 a);
void printVec3(Vec3 a);
void printVec4(Vec4 a);
void printMat3(Mat3 a);
void printMat4(Mat4 a);


#endif