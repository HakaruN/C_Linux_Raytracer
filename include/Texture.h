#ifndef __TEXTURE_H__
#define __TEXTURE_H__
#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include "../vendor/stb/stb_image.h"
#include <string.h>

#include <stdio.h>

typedef struct Texture {
  unsigned int width, height;
  unsigned char channels;
  unsigned char* image;
  unsigned int sizeBytes;
} Texture;

Texture loadTexture(char* filename, int desiredChannels);
void freeTexture(Texture* tex);


#endif
