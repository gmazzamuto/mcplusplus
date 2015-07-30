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

#include <MCPlusPlus/MCglobal.h>

namespace MCPP {

uint walkerFlagToType(walkerFlags flag)
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


walkerFlags walkerTypeToFlag(uint index)
{
    return (walkerFlags)(1 << index);
}


string walkerTypeToString(uint index)
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

unsigned int walkerSaveFlags(const string flags) {
    unsigned int ret = 0;
    for (unsigned int i = 0; i < flags.size(); ++i) {
        switch (flags.at(i)) {
        case 't':
            ret |= FLAG_TRANSMITTED;
            break;
        case 'b':
            ret |= FLAG_BALLISTIC;
            break;
        case 'r':
            ret |= FLAG_REFLECTED;
            break;
        case 'k':
            ret |= FLAG_BACKREFLECTED;
            break;
        default:
            break;
        }
    }

    return ret;
}

}
