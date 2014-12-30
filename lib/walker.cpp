#include "walker.h"
#include <string.h> //for memset()

using namespace MCPP;

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

void Walker::reset()
{
    memset(r0,0,3*sizeof(MCfloat));
    memset(r1,0,3*sizeof(MCfloat));
    memset(k0,0,3*sizeof(MCfloat));
    memset(k1,0,3*sizeof(MCfloat));
    walkTime = 0;
}
