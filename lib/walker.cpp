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
