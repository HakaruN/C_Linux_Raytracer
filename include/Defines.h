
#ifndef __DEBUG_H__
#define __DEBUG_H__

#define DEBUG

//#define FIXED_POINT

#ifdef DEBUG
#include <stdio.h>
#endif

#ifdef DEBUG
//32 bit fx numbers, split with the integer representing 14 bits
#define FPT_BITS 32
#define FPTW_BITS 14
#include "fptc.h"
#endif

#endif
