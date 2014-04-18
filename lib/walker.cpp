#include "walker.h"
#include <stdlib.h>
#include <string.h> //for memset()

Walker::Walker()
{
    r0 = (MCfloat*)calloc(3,sizeof(MCfloat));
    r1 = (MCfloat*)calloc(3,sizeof(MCfloat));
    k0 = (MCfloat*)calloc(3,sizeof(MCfloat));
    k1 = (MCfloat*)calloc(3,sizeof(MCfloat));
    walkTime = 0;
}

Walker::~Walker()
{
    free(r0);
    free(r1);
    free(k0);
    free(k1);
}

void Walker::swap_r0_r1()
{
    MCfloat *temp = r1;
    r1 = r0;
    r0 = temp;
}

void Walker::swap_k0_k1()
{
    MCfloat *temp = k1;
    k1 = k0;
    k0 = temp;
}

void Walker::reset()
{
    memset(r0,0,3*sizeof(MCfloat));
    memset(r1,0,3*sizeof(MCfloat));
    memset(k0,0,3*sizeof(MCfloat));
    memset(k1,0,3*sizeof(MCfloat));
    walkTime = 0;
}
