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

#ifndef COSTHETAGENERATOR_H
#define COSTHETAGENERATOR_H
#include "distributions.h"

namespace MCPP {

/**
 * @brief Spins the value of \f$ \cos \theta \f$ after a scattering event in
 * accordance with given anisotropy factor \f$ g \f$
 */

class CosThetaGenerator : public AbstractDistribution
{
public:
    CosThetaGenerator(double g=0, BaseObject *parent=NULL);

    void setg(double g);
    MCfloat spin() const;

private:
    virtual BaseObject *clone_impl() const;
    MCfloat g;  /**< @brief scattering anisotropy factor*/
};

}
#endif // COSTHETAGENERATOR_H
