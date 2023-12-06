#ifndef __FRAMEBUFFER_H__
#define __FRAMEBUFFER_H__
#include "Defines.h"
#include "Maths.h"
#include "Triangle.h"
#include <stdlib.h>
#ifdef DEBUG
#include <stdio.h>
#endif

//indices into the FB descriptor to access the elements of the FB descriptor
#define WIDTH 0
#define HEIGHT 1
#define COLOURS_PER_PIXEL 2
typedef unsigned int FbDescriptor[3];

typedef unsigned char* FrameBuffer;
typedef Triangle** RayHitBuffer;//one entry per pixel (or ray), each pixel now knows what triangle it hit
//typedef unsigned short* RayHitBuffer;//one entry per pixel (or ray), each pixel now knows what it's ray hit.
typedef Vec3* RayHitpointBuffer;//one entry per pixel (pr ray), where in 3D space the ray hit.
typedef Vec3* RayHitNormalBuffer;//stores the normal of the triangle intersected
typedef Vec3* RayHitDirectionBuffer;//stores the direction of the ray intersecting the triangle

FrameBuffer createFB(FbDescriptor fbDescriptor);
void freeFB(FrameBuffer fb);

#endif
