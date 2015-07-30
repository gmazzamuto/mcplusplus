/*
  This file is part of MCPlusPlus.

  MCPlusPlus is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Mcplusplus is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with MCPlusPlus.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MCGLOBAL_H
#define MCGLOBAL_H

#include <iostream>
#include <cstdlib>

#define DOUBLEPRECISION

//#undef DOUBLEPRECISION

#ifdef DOUBLEPRECISION
    typedef double MCfloat;
#endif

#ifdef LONGDOUBLEPRECISION
    typedef long double MCfloat;
#endif

#ifdef SINGLEPRECISION
    typedef float MCfloat;
#endif

/*! \file */

namespace MCPP {

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

enum MCData {
    DATA_NONE,
    DATA_POINTS,
    DATA_K,
    DATA_TIMES,
};

#define MC_ASSERT_MSG(x, msg) if(!x) { cerr << "=========== ERROR ===========" << endl; cerr << "msg" << endl; abort(); }

extern uint walkerFlagToType(walkerFlags flag);
extern walkerFlags walkerTypeToFlag(uint index);
extern string walkerTypeToString(uint index);
extern unsigned int walkerSaveFlags(const string flags);
}

#endif // MCGLOBAL_H
