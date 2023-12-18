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

#define ERROR 0
#define ALREADY_PRESENT 1
#define TEXTURE_NOT_PRESENT 2
#define TEXTURE_ADDED 2


typedef struct Texture {
  unsigned int width, height;
  unsigned char channels;
  unsigned char *image;
  unsigned int sizeBytes;
  char *name;
} Texture;

typedef struct Textures {
  unsigned int numTextures, maxTextures;
  Texture* textures;
} Textures;

typedef struct Material {

} Material;

////TEXTURE
Texture* loadTexture(char* filename, int desiredChannels);
void texturefreeImage(Texture* tex);


////TEXTURE CONTAINER
Textures* textureGenTextures(unsigned int num);//get me a container
unsigned int textureAddTex(Textures* container, char* filename, int desiredChannels, unsigned int* index);//Add a texture to the container. Index returns the index of the newly added texture in the container
unsigned int textureIsPresent(Textures* container, char* filename, unsigned int* index);//Is the texture already in the container? index param return the index in the container for the texture



#endif
