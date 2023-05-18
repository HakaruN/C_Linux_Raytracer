#ifndef __TEXTURE_H__
#define __TEXTURE_H__
#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include "../vendor/stb/stb_image.h"
#include <string.h>

#include <stdio.h>

unsigned char* loadTexture(char* filename);



#endif
