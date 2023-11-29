#include "../include/FP.h"

///Unsigned fixed point
UFX16_16 ufloatToFixed(float f)
{
    return (uint32_t)(f * (float)(1 << POST_DP_BITS) + (f >= 0 ? 0.5 : -0.5));
}

float ufixedToFloat(UFX16_16 f)
{
    return (float)f / (float)(1 << POST_DP_BITS);
}

UFX16_16 uintToUfixed(unsigned int a)
{
    return a << 16;
}
unsigned int uFixedToUint(UFX16_16 a)
{
    return a >> 16;
}

UFX16_16 uFXMul(UFX16_16 a, UFX16_16 b)
{
    return ((uint64_t)a * (uint64_t)b) >> POST_DP_BITS;
}

UFX16_16 uFXDiv(UFX16_16 a, UFX16_16 b)
{
    return ((uint64_t)a << POST_DP_BITS) / (uint64_t)b;
}


///Signed fixed point
SFX16_16 sfloatToFixed(float f)
{
    return (int32_t)(f * (float)(1 << POST_DP_BITS) + (f >= 0 ? 0.5 : -0.5));
}

float sfixedToFloat(SFX16_16 f)
{
    return (float)f / (float)(1 << POST_DP_BITS);
}

SFX16_16 sintToSfixed(int a)
{
    return a << 16;
}

int sFixedToSint(SFX16_16 a)
{
    return a >> 16;
}

SFX16_16 sFXMul(SFX16_16 a, SFX16_16 b)
{
    return ((int64_t)a * (int64_t)b) >> POST_DP_BITS;
}

SFX16_16 sFXDiv(SFX16_16 a, SFX16_16 b)
{
    if(!a)
        return 0;
    if(b == 0)
        return 0b01111111111111111111111111111111;
    return ((int64_t)a << POST_DP_BITS) / (int64_t)b;
}