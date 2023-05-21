#include "../include/FrameBuffer.h"


FrameBuffer createFB(FbDescriptor fbDescriptor)
{
    //create the framebuffer on the heap
  FrameBuffer fb = malloc(sizeof(unsigned char) *
			  fbDescriptor[WIDTH] *
			  fbDescriptor[HEIGHT] *
			  fbDescriptor[COLOURS_PER_PIXEL]);
#ifdef DEBUG
  if(fb)
    printf("Created Framebuffer, allocated %u KiB.\n", (fbDescriptor[WIDTH] * fbDescriptor[HEIGHT] * fbDescriptor[COLOURS_PER_PIXEL])/1024);
  else
    printf("Failed to create Framebuffer\n");
 #endif

  if(fb)
    return fb;
}

void freeFB(FrameBuffer fb)
{
  if(fb)
    free(fb);
#ifdef DEBUG
  printf("Freed framebuffer\n");
#endif
}
