#include "../include/Texture.h"

/// CONTAINER
Textures* textureGenTextures(unsigned int num)
{
  //allocate texture container
  Textures* texs = malloc(sizeof(Textures));
  if(texs)
  {
    //now allocate space for the textures in the container
    texs->textures = malloc(sizeof(Texture) * num);
    if(texs->textures)
    {
      texs->maxTextures = num;
      texs->numTextures = 0;
    }
    else
    {
      free(texs);
      texs = NULL;
    }
  }
  return texs;
}

unsigned int textureAddTex(Textures* container, char* filename, int desiredChannels, unsigned int* index)
{
  if(container)
  {
    //check if we need to allocate more space for textures
    if(!(container->numTextures < container->maxTextures))
    {
      //make more space
      unsigned int moreToAdd = (container->maxTextures / 5) + 1;//add 20% + 1 so we will always allocate at least one more
      unsigned int numToAllocate = container->maxTextures + moreToAdd;
      Texture* texTemp = malloc(sizeof(Texture) * numToAllocate);
      if(texTemp)
      {
        memset(texTemp, 0, sizeof(Texture) * numToAllocate);//null out the whole buffer
        memcpy(texTemp, container->textures, sizeof(Texture) * container->numTextures);//copy the existing texs to the buffer
        free(container->textures);//clear old buffer
        container->textures = texTemp;//set old buffer handle to new buffer
        container->maxTextures += moreToAdd;//update the new max
      }
      else
      {
        #ifdef DEBUG
        printf("Error allocating space for new texture buffer\n");
        #endif
        return ERROR;
      }
    }
    ///Add new texture
    //first check if it's already present
    unsigned int result = textureIsPresent(container, filename, index);
    if(result != TEXTURE_NOT_PRESENT)
      return result;//either it errored or it was already present

    //add the texture if it's not presetnt and we didn't error
    
    int width, height, channels;  stbi_set_flip_vertically_on_load(1);
    Texture* texture = &container->textures[container->numTextures];
    texture->image = stbi_load(filename, &width, &height, &channels, desiredChannels);
    if(texture->image)
    {
      #ifdef DEBUG
        printf("Loaded %s, width: %d, height: %d, channels: %d\n",filename, width, height, channels);
      #endif
      texture->width = width; texture->height = height;
      texture->channels = channels;
      texture->sizeBytes = width * height * channels;
      //get size of file name      
      unsigned int len = strlen(filename);
      texture->name = malloc(len * sizeof(char));
      strcpy(texture->name, filename);
    }
    else
    {
      #ifdef DEBUG
      printf("Failed to load texture: %s\n", filename);
      #endif
      container->textures[container->numTextures].image = NULL;
      return ERROR;
    }
    *index = container->numTextures;
    container->numTextures++;
    return TEXTURE_ADDED;
  }
  return ERROR;
}


inline unsigned int textureIsPresent(Textures* container, char* filename, unsigned int* index)//Is the texture already in the container? 
{
  //check if the texture is already in the list
  if(container)
  {
    for(unsigned int i = 0; i < container->numTextures; i++)
    {
      if(container->textures[i].name == filename)
      {
        *index = i;
        return ALREADY_PRESENT;
      }
    }
    return TEXTURE_NOT_PRESENT;
  }
  else
    return ERROR;
}

///TEXTURE

Texture* loadTexture(char* filename, int desiredChannels)
{
  int width, height, channels;  stbi_set_flip_vertically_on_load(1);
  Texture* t = malloc(sizeof(Texture));
  if(t)
  {
  printf("Loading %s\n",filename);
  t->image = stbi_load(filename, &width, &height, &channels, desiredChannels);

  t->width = width; t->height = height; t->channels = channels;
  t->sizeBytes = width * height * channels;
  t->name = filename;
  if(t->image)
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
      t->image = NULL;
    }
  }
  return t;
}

void texturefreeImage(Texture* tex)
{
  free(tex->name);
  stbi_image_free(tex->image);
  free(tex);
}
