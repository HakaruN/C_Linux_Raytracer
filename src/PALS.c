#include "../include/PALS.h"


ColourPAL* initColourPAL(unsigned int numEntries)
{
  if(numEntries > 0)
    {
#ifdef DEBUG_PAL
      printf("Initialising colour PAL for %u entries.\n", numEntries);
#endif

      ColourPAL* cp = malloc(sizeof(ColourPAL));
      if(cp)
	{
	  cp->reds = malloc(sizeof(unsigned char) * numEntries);
	  cp->greens = malloc(sizeof(unsigned char) * numEntries);
	  cp->blues = malloc(sizeof(unsigned char) * numEntries);
	  cp->size = numEntries; cp->entries = 0;

	  if(cp->reds && cp->greens && cp->blues)
	    {
#ifdef DEBUG_PAL
	      printf("Initialised colour PAL. Allocated %u bytes.\n", 3 * sizeof(unsigned char) * numEntries + sizeof(ColourPAL));
#endif
	      return cp;
	    }
	}

#ifdef DEBUG_PAL
      printf("Failed to allocate the PAL\n");
#endif
      return NULL;

    }
}

unsigned int addColour(ColourPAL* PAL, unsigned char* colour)
{
  if(PAL)
    {
      if(colour)
	{
#ifdef DEBUG_PAL
	  printf("Adding colour to PAL: %u, %u, %u\n", colour[0], colour[1], colour[2]);
#endif
	  unsigned int numEntries = getCPALNumEntries(PAL) + 1;
	  if(numEntries <= getCPALSize(PAL))
	    {
	      PAL->reds[numEntries] = colour[0];
	      PAL->greens[numEntries] = colour[1];
	      PAL->blues[numEntries] = colour[2];
	      PAL->entries++;
	      return numEntries;
	    }
	  else
	    {
#ifdef DEBUG_PAL
	      printf("ERROR PAL full, not adding colour.\n");
#endif
	    }
	}
      else
	{
#ifdef DEBUG_PAL
	  printf("ERROR Invalid colour provided\n");
#endif
	}
    }
  else
    {
#ifdef DEBUG_PAL
      printf("ERROR Invalid PAL provided\n");
#endif
    }
  return 0;
}

unsigned int addColourRGB(ColourPAL* PAL, unsigned char r, unsigned char g, unsigned char b)
{
  if(PAL)
    {
      #ifdef DEBUG_PAL
      printf("Adding colourRGB\n");
      #endif
      unsigned int numEntries = getCPALNumEntries(PAL);
      if(numEntries <= getCPALSize(PAL))
	{
	  PAL->reds[numEntries] = r;
	  PAL->greens[numEntries] = g;
	  PAL->blues[numEntries] = b;
	  PAL->entries++;
	  return numEntries;
	}
      else
	{
#ifdef DEBUG_PAL
	  printf("ERROR PAL full, not adding colour.\n");
#endif
	}
    }
  else
    {
#ifdef DEBUG_PAL
      printf("ERROR Invalid PAL provided\n");
#endif
    }
  return 0;
}

Colour3 getColour(ColourPAL* PAL, unsigned int idx)
{
  if(PAL)
    {
      if(idx <= getCPALNumEntries(PAL))
	{
#ifdef DEBUG_PAL
      printf("Getting colour at: %u. RGB:%u,%u,%u\n", idx, PAL->reds[idx], PAL->greens[idx], PAL->blues[idx]);
#endif
      return (Colour3){idx, &PAL->reds[idx], &PAL->greens[idx], &PAL->blues[idx]};
	}
      else
	{
#ifdef DEBUG_PAL
	  printf("ERROR index out of PALs bounds\n");
#endif
	}
    }
  else
    {
#ifdef DEBUG_PAL
      printf("ERROR uninitialised PAL supplied\n");
#endif
      return (Colour3){0,NULL, NULL, NULL};
    }
}

unsigned int getCPALSize(ColourPAL* PAL)
{
  if(PAL)
    {
#ifdef DEBUG_PAL
      printf("Sizeof PAL: %u\n", PAL->size);
#endif
      return(PAL->size);
    }
  else
    {
#ifdef DEBUG_PAL
      printf("ERROR uninitialised PAL supplied\n");
#endif
      return 0;
    }
}
unsigned int getCPALNumEntries(ColourPAL* PAL)
{
  if(PAL)
    {
#ifdef DEBUG_PAL
      printf("Num entires in PAL:  %u\n", PAL->entries);
#endif
      return(PAL->entries);
    }
    else
      {
#ifdef DEBUG_PAL
	printf("ERROR uninitialised PAL supplied\n");
#endif
	return 0;
      }
}
ColourPAL* resizeCPAL(ColourPAL* PAL, unsigned int numExtraEntries)
{
  if(PAL)
    {
      #ifdef DEBUG_PAL
      printf("Resizing PAL by +%u entries\n", numExtraEntries);
      #endif
      if(PAL->reds && PAL->greens && PAL->blues)
	{
	  unsigned int entries = PAL->entries;
	  //Create a new ColourPAL with the additional size
	  ColourPAL* newPAL = initColourPAL(PAL->size + numExtraEntries);
	  if(newPAL)
	    {
	      //copy the sizing info
	      newPAL->size = PAL->size + numExtraEntries;
	      newPAL->entries = PAL->entries;
	      //copy the contents from old PAL to new
	      memcpy(newPAL->reds, PAL->reds, entries * sizeof(unsigned char));
	      memcpy(newPAL->greens, PAL->greens, entries * sizeof(unsigned char));
	      memcpy(newPAL->blues, PAL->blues, entries * sizeof(unsigned char));
	      //free old PAL and return new PAL
	      freeCPAL(PAL);
	      return newPAL;
	    }
	  else
	    {
#ifdef DEBUG_PAL
	      printf("Error. Could not create a new extended PAL\n");
#endif
	      return NULL;
	    }
	}
      else
	{
#ifdef DEBUG_PAL
      printf("Error. PAL incorrectly initialised. Not extending PAL\n", numExtraEntries);
#endif
	  return NULL;
	}
    }
  else
    {
#ifdef DEBUG_PAL
      printf("Error. PAL supplied is NULL. Not extending PAL\n", numExtraEntries);
#endif
      return NULL;
    }
}

void freeCPAL(ColourPAL* PAL)
{
  if(PAL)
    {
      #ifdef DEBUG_PAL
      printf("Freeing PAL\n");
      #endif
      if(PAL->reds)
	free(PAL->reds);
      if(PAL->greens)
	free(PAL->greens);
      if(PAL->blues)
	free(PAL->blues);

      #ifdef DEBUG_PAL
      printf("Freeing %d bytes.\n", sizeof(ColourPAL) + (3 * sizeof(unsigned char) * PAL->size));
      #endif
      free(PAL);
    }
  else
    {
#ifdef DEBUG_PAL
      printf("PAL not initialised. Not freeing\n");
#endif
    }
}
