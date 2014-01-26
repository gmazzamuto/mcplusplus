#include "walker.h"

Walker::Walker()
{
    r0[0]=r0[1]=r0[2]=0;
    k0[0]=k0[1]=k0[2]=0;
    r1[0]=r1[1]=r1[2]=0;
    k1[0]=k1[1]=k1[2]=0;
    walkTime = 0;
    nInteractions.push_back(0);
}
