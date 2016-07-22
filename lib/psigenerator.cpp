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

#include <MCPlusPlus/psigenerator.h>

#include <boost/math/constants/constants.hpp>

using namespace boost::math::constants;
using namespace MCPP;

IsotropicPsiGenerator::IsotropicPsiGenerator(BaseObject *parent) :
    AbstractDistribution(parent)
{

}

MCfloat IsotropicPsiGenerator::spin() const {
    return uniform_01<MCfloat>()(*mt) * two_pi<MCfloat>(); //uniform in [0,2pi)
}

BaseObject *IsotropicPsiGenerator::clone_impl() const
{
    return new IsotropicPsiGenerator();
}
