#include "../include/Texture.h"

Texture loadTexture(const char* filename, int desiredChannels)
{
  int width, height, channels;  stbi_set_flip_vertically_on_load(1);
  Texture t;
  printf("Loading %s\n",filename);
  t.image = stbi_load(filename, &width, &height, &channels, desiredChannels);

  t.width = width; t.height = height; t.channels = channels;
  t.sizeBytes = width * height * channels;

  if(t.image)
    {
      #ifdef DEBUG
      printf("Loaded %s, width: %d, height: %d, channels: %d\n",filename, width, height, channels);
      #endif
    }
  else
    {
      #ifdef DEBUG
      printf("Failed to load texture: %s\n", filename);
      #endif
      t.image = NULL;
    }
  return t;
}

void freeTex(Texture* tex)
{
}
