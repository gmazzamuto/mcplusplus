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

#ifndef PSIGENERATOR_H
#define PSIGENERATOR_H
#include "distributions.h"

namespace MCPP {

/**
 * @brief The IsotropicPsiGenerator class generates the azimuthal scattering
 * angle uniformly in the interval \f$ \psi \in [0, 2\pi) \f$.
 */

class IsotropicPsiGenerator : public AbstractDistribution
{
public:
    IsotropicPsiGenerator(BaseObject *parent=NULL);

    virtual MCfloat spin() const;

private:
    virtual BaseObject* clone_impl() const;
};

}
#endif // PSIGENERATOR_H
