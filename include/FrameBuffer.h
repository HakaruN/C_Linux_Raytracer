#ifndef __FRAMEBUFFER_H__
#define __FRAMEBUFFER_H__
#include "Defines.h"
#include <stdlib.h>
#ifdef DEBUG
#include <stdio.h>
#endif

//indices into the FB descriptor to access the elements of the FB descriptor
#define WIDTH 0
#define HEIGHT 1
#define COLOURS_PER_PIXEL 2

typedef unsigned char* FrameBuffer;

FrameBuffer createFB(int* fbDescriptor);
void freeFB(FrameBuffer fb);

#endif
