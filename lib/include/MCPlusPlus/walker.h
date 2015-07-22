/*
  This file is part of MCPlusPlus.

  MCPlusPlus is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Foobar is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with MCPlusPlus.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef WALKER_H
#define WALKER_H

#include "MCglobal.h"
#include <vector>

namespace MCPP {

using namespace std;

class Walker
{
public:
    Walker();
    ~Walker();

    void reset();

    MCfloat r0[3];  /**< @brief position before the \f$ n \f$-th step*/
    MCfloat k0[3];  /**< @brief direction unit vector for the \f$ (n-1) \f$-th step*/
    MCfloat walkTime;  /**< @brief total walk time */
    int type;
};

}
#endif // WALKER_H
