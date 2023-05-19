#ifndef __CAMERA_H__
#define __CAMERA_H__
#include "Maths.h"

typedef struct{
  Vec3 origin;
  Vec3 forwards;
  Vec3 up;
  Vec3 right;
  //  Vec3 cameraLook;
  Vec2 dimensions;//width = [0], height = [1]
  float aspectRatio;
  float fov;
} Camera;

Camera cameraGen(Vec3 origin, Vec3 target, Vec3 up, float fov, float aspectRatio);
void update(Camera* c, Vec3 origin, Vec3 target, Vec3 up, float fov, float aspectRation);


#endif
