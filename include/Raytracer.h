#ifndef __RAYTRACER_H__
#define __RAYTRACER_H__
#include "BvhNode.h"
#include "Triangle.h"
#include "Camera.h"
#include "Ray.h"
#include "FrameBuffer.h"
#include "Window.h"
#include "BvhNode.h"
#include "Box.h"
#include "Texture.h"



void traceRays(Triangle* triangles, unsigned int numTriangles, FrameBuffer frameBuffer, Camera* camera, RayHitBuffer rayHitBuffer, RayHitpointBuffer rayHitPointBuffer, FbDescriptor fbDescriptor, float invHeightMinus1, float invWidthMinus1);

#endif
