#include "../include/Texture.h"

Texture loadTexture(char* filename, int desiredChannels)
{
  int width, height, channels;  stbi_set_flip_vertically_on_load(1);
  Texture t;
  printf("Loading %s\n",filename);
  t.image = stbi_load(filename, &width, &height, &channels, desiredChannels);

  t.width = width; t.height = height; t.channels = channels;

  if(t.image)
    {
      printf("Loaded %s, width: %d, height: %d, channels: %d\n",filename, width, height, channels);
    }
  else
    t.image = NULL;
  return t;
}

void freeTex(Texture* tex)
{
}
