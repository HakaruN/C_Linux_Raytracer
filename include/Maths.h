#ifndef __MATHS_H__
#define __MATHS_H__


#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#ifdef ALTIVEC
#include <altivec.h>
#endif


typedef float Vec1;
typedef float Vec2[2];
typedef float Vec3[3];
typedef float Vec4[4];

typedef Vec2 Mat2[2];
typedef Vec3 Mat3[3];
typedef Vec4 Mat4[4];

float degToRad(float deg);
float radToDeg(float rad);

void cross(Vec3 a, Vec3 b, Vec3 result);
float vec3Len(Vec3 a);
float vec4Len(Vec4 a);
float vec3Dot(Vec3 a, Vec3 b);
float vec4Dot(Vec4 a, Vec4 b);

//Gets a projection of b onto a as vector res
//TODO: Reimplement orthog projection to take axis to project onto
//void vec3OrthogProjec(Vec3 a, Vec3 b, Vec3 res);
//void vec4OrthogProjec(Vec4 a, Vec4 b, Vec4 res);

void vec3Normalise(Vec3 vec, Vec3 result);
void vec4Normalise(Vec4 vec, Vec4 result);
void vec3Add(Vec3 a, Vec3 b, Vec3 result);
void vec4Add(Vec4 a, Vec4 b, Vec4 result);
void vec3Sub(Vec3 a, Vec3 b, Vec3 result);
void vec4Sub(Vec4 a, Vec4 b, Vec4 result);
void vec3PairwiseMult(Vec3 a, Vec3 b, Vec3 result);
void vec4PairwiseMult(Vec4 a, Vec4 b, Vec4 result);
void vec3ScalarMult(Vec3 vec, float scalar, Vec3 result);
void vec4ScalarMult(Vec4 vec, float scalar, Vec4 result);
float vec3Displacement(Vec3 a, Vec3 b);
float vec4Displacement(Vec4 a, Vec4 b);
float angleBetweenVec3(Vec3 a, Vec3 b);
float angleBetweenVec4(Vec3 a, Vec3 b);

void mat3Mul(Mat3 a, Mat3 b, Mat3 c);
void mat4Mul(Mat4 a, Mat4 b, Mat4 c);
void mat3VecMul(Mat3 a, Vec3 b, Vec3 res);
void mat4VecMul(Mat4 a, Vec4 b, Vec4 res);
void mat3GetUnitMat(Mat3 res);
void mat4GetUnitMat(Mat4 res);
void mat4GetTranslateMat(Vec3 t, Mat4 res);
void mat4GetScaleMat(Vec3 t, Mat4 res);
void mat4GetRotateXMat(float angle, Mat4 res);
void mat4GetRotateYMat(float angle, Mat4 res);
void mat4GetRotateZMat(float angle, Mat4 res);


float normalise(float val, float fromMin, float fromMax, float toMin, float toMax);

void printVec2(Vec2 a);
void printVec3(Vec3 a);
void printVec4(Vec4 a);
void printMat3(Mat3 a);
void printMat4(Mat4 a);


#endif

