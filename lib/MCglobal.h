#ifndef MCGLOBAL_H
#define MCGLOBAL_H

#include <iostream>
#include <cstdlib>

using namespace std;

typedef double MCfloat;

enum walkerType {
    TRANSMITTED = 0,
    BALLISTIC = 1,
    REFLECTED = 2,
    BACKREFLECTED = 3,
};

enum directions {
    DIR_X = 1 << 0,
    DIR_Y = 1 << 1,
    DIR_Z = 1 << 2,
    ALL_DIRS = 0xf,
};

#define MC_ASSERT_MSG(x, msg) if(!x) { cerr << "=========== ERROR ===========" << endl; cerr << "msg" << endl; abort(); }

#endif // MCGLOBAL_H
