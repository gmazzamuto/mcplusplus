#ifndef MCGLOBAL_H
#define MCGLOBAL_H

#include <iostream>
#include <cstdlib>

#ifndef DOUBLEPRECISION
#define DOUBLEPRECISION
#endif

//#undef DOUBLEPRECISION

#ifdef DOUBLEPRECISION
typedef double MCfloat;
#else
typedef float MCfloat;
#endif

using namespace std;


enum walkerType {
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

enum DataGroup {
    DATA_POINTS,
    DATA_K,
    DATA_TIMES,
};

#define MC_ASSERT_MSG(x, msg) if(!x) { cerr << "=========== ERROR ===========" << endl; cerr << "msg" << endl; abort(); }

extern uint walkerFlagToType(walkerFlags flag);
extern walkerFlags walkerTypeToFlag(uint index);
extern string walkerTypeToString(uint index);

#endif // MCGLOBAL_H
