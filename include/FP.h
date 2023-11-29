#ifndef __FP_H__
#define __FP_H__
#include <stdint.h>

#define FX

#define POST_DP_BITS 16
#define ItoFX(i) i << POST_DP_BITS
#define FXtoI(i) i >> POST_DP_BITS
//16_16 fixed point numbers
typedef uint32_t UFX16_16;
typedef int32_t SFX16_16;

///Unsigned fixed point
UFX16_16 ufloatToFixed(float f);
float ufixedToFloat(UFX16_16 f);

UFX16_16 uintToUfixed(unsigned int a);
unsigned int uFixedToUint(UFX16_16 a);

UFX16_16 uFXMul(UFX16_16 a, UFX16_16 b);
UFX16_16 uFXDiv(UFX16_16 a, UFX16_16 b);

///Signed fixed point
SFX16_16 sfloatToFixed(float f);
float sfixedToFloat(SFX16_16 f);

SFX16_16 sintToSfixed(int a);
int sFixedToSint(SFX16_16 a);

SFX16_16 sFXMul(SFX16_16 a, SFX16_16 b);
SFX16_16 sFXDiv(SFX16_16 a, SFX16_16 b);

/*Examples 

    UFX16_16 ua = ufloatToFixed(60000.f);
    UFX16_16 ub = ufloatToFixed(1.05f);
    float ui = ufixedToFloat(ua);
    float uj = ufloatToFixed(ub);

    UFX16_16 uc = uFXMul(ua, ub);
    float uk = ufixedToFloat(uc);

    UFX16_16 ud = uFXDiv(ua, ub);
    float ul = ufixedToFloat(ud);

    SFX16_16 sa = sfloatToFixed(60000.f);
    SFX16_16 sb = sfloatToFixed(1.05f);
    float si = sfixedToFloat(sa);
    float sj = sfloatToFixed(sb);

    SFX16_16 sc = sFXMul(sa, sb);
    float sk = sfixedToFloat(sc);

    SFX16_16 sd = sFXDiv(sa, sb);
    float sl = sfixedToFloat(sd);

    float uab = ufixedToFloat(ua + ub);


*/

#endif