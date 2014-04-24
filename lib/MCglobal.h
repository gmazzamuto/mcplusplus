#ifndef MCGLOBAL_H
#define MCGLOBAL_H

#include <iostream>
#include <cstdlib>

using namespace std;

typedef double MCfloat;

enum walkerIndex {
    TRANSMITTED = 0,
    BALLISTIC = 1,
    REFLECTED = 2,
    BACKREFLECTED = 3,
};

enum walkerFlags {
    FLAG_TRANSMITTED = 1 << TRANSMITTED,
    FLAG_BALLISTIC = 1 << BALLISTIC,
    FLAG_REFLECTED = 1 << REFLECTED,
    FLAG_BACKREFLECTED = 1 << BACKREFLECTED,
    FLAG_ALL_WALKERS = 0xf,
};

enum directions {
    DIR_X = 1 << 0,
    DIR_Y = 1 << 1,
    DIR_Z = 1 << 2,
    ALL_DIRS = 0xf,
};

#define MC_ASSERT_MSG(x, msg) if(!x) { cerr << "=========== ERROR ===========" << endl; cerr << "msg" << endl; abort(); }

extern uint walkerFlagToIndex(walkerFlags flag);
extern uint walkerIndexToFlag(uint index);

#endif // MCGLOBAL_H
