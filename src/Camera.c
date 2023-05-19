#include "../include/Camera.h"


Camera cameraGen(Vec3 origin, Vec3 target, Vec3 up, float fov, float aspectRatio)
{
  Camera c;
  c.fov = fov;
  memcpy(c.origin, origin, 3 * sizeof(float));//copy origin

  //calc forwards direction
  vec3Sub(target, origin, c.forwards);
  vec3Normalise(c.forwards, c.forwards);

  //calc right
  cross(c.forwards, up, c.right);
  vec3Normalise(c.right, c.right);

  //calc up
  cross(c.right, c.forwards, c.up);

  c.aspectRatio = aspectRatio;

  float fovToRad = degToRad(fov);
  c.dimensions[1] = tan(fovToRad);
  c.dimensions[0] = c.dimensions[1] * c.aspectRatio;

  return c;
}


inline void update(Camera* c, Vec3 origin, Vec3 target, Vec3 up, float fov, float aspectRatio)
{
  c->fov = fov;
  memcpy(c->origin, origin, 3 * sizeof(float));

  vec3Sub(target, origin, c->forwards);
  vec3Normalise(c->forwards, c->forwards);

  cross(c->forwards, up, c->right);
  vec3Normalise(c->right, c->right);

  cross(c->right, c->forwards, c->up);
  c->aspectRatio = aspectRatio;
  float fovToRad = degToRad(fov);
  c->dimensions[1] = tan(fovToRad);
  c->dimensions[0] = c->dimensions[1] * c->aspectRatio;

}
