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

/*! \file */

using namespace std;

/**
 * @brief The walkerType enum enumerates the possible photon classifications
 * based on how the photon exited the sample.
 *
 * This enum is also intended to be used as index for arrays or vectors
 * containing data for each photon type.
 */

enum walkerType {
    TRANSMITTED = 0,
    BALLISTIC = 1,
    REFLECTED = 2,
    BACKREFLECTED = 3,
};

/**
 * @brief The walkerFlags enum is used to select one or more photon types.
 *
 * This enum is intended to be used in a bitwise fashion only.
 *
 * \warning Do not use this enum as an index for arrays or vectors. Use
 * #walkerType instead.
 */

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
