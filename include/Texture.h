#ifndef __TEXTURE_H__
#define __TEXTURE_H__
#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_BMP
#define STBI_NO_PSD
#define STBI_NO_TGA
#define STBI_NO_GIF
#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_PNM
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
