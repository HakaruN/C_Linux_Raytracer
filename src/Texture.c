#include "../include/Texture.h"

unsigned char* loadTexture(char* filename)
{
  int width, height, channels;
  printf("Loading %s\n",filename);
  unsigned char* buff = stbi_load(filename, &width, &height, &channels, 0);
  if(buff)
    {
      printf("Loaded %s, width: %d, height: %d, channels: %d\n",filename, width, height, channels);
    return buff;
    }
  else
    return NULL;
}
