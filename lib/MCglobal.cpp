#include "MCglobal.h"

uint walkerFlagToIndex(walkerFlags flag)
{
    uint ret = 0xffffffff;
    if(ret != FLAG_ALL_WALKERS) {
        for (uint i = 0; i < 4; ++i) {
            if(flag >> i == 1) {
                ret = i;
                break;
            }
        }
    }
    return ret;
}


uint walkerIndexToFlag(uint index)
{
    return 1 << index;
}


string walkerIndexToString(uint index)
{
    switch (index) {
    case TRANSMITTED:
        return "transmitted";

    case BALLISTIC:
        return "ballistic";

    case REFLECTED:
        return "reflected";

    case BACKREFLECTED:
        return "back-reflected";

    default:
        return "";
        break;
    }
}
