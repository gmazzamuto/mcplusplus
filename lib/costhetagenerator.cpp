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

#include <MCPlusPlus/costhetagenerator.h>

using namespace MCPP;

CosThetaGenerator::CosThetaGenerator(double g, BaseObject *parent) :
    AbstractDistribution(parent)
{
    setg(g);
}

void CosThetaGenerator::setg(double g) {
    this->g = g;
}

MCfloat CosThetaGenerator::spin() const {
    if(g==0.) {
        return uniform_01<MCfloat>()(*mt)*2.-1.; //uniform in [-1,1)
    }
    else {
        MCfloat temp;
        const MCfloat xi = uniform_01<MCfloat>()(*mt);
        temp = 1 - g + 2*g*xi;
        temp = (1-g*g)/temp;
        temp = 1 + g*g - temp*temp;
        temp = temp/(2*g);
        if(temp < -1.)
            temp = -1.;
        else if(temp > 1.)
            temp = 1.;
        return temp;
    }
}

BaseObject *CosThetaGenerator::clone_impl() const
{
    return new CosThetaGenerator(g);
}
