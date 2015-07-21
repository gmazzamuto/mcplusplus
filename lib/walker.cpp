#include <MCPlusPlus/walker.h>
#include <string.h> //for memset()

using namespace MCPP;

Walker::Walker()
{
    reset();
}

Walker::~Walker()
{
}

void Walker::reset()
{
    memset(r0,0,3*sizeof(MCfloat));
    memset(k0,0,3*sizeof(MCfloat));
    walkTime = 0;
    type = -1;
}
