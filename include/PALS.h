#ifndef __PALS_H__
#define __PALS_H__

#include "Defines.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
  unsigned int size, entries;
  unsigned char* reds, *greens, *blues;
} ColourPAL;

typedef struct {
  unsigned int CPALIdx;
  unsigned char *red, *green, *blue;
} Colour3;

ColourPAL* initColourPAL(unsigned int numEntries);
unsigned int addColour(ColourPAL* PAL, unsigned char* colour);
unsigned int addColourRGB(ColourPAL* PAL, unsigned char r, unsigned char g, unsigned char b);
Colour3 getColour(ColourPAL* PAL, unsigned int idx);
unsigned int getCPALSize(ColourPAL* PAL);
unsigned int getCPALNumEntries(ColourPAL* PAL);
ColourPAL* resizeCPAL(ColourPAL* PAL, unsigned int numExtraEntries);
void freeCPAL(ColourPAL* PAL);
#endif
