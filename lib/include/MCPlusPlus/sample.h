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

#ifndef SAMPLE_H
#define SAMPLE_H

#include "baseobject.h"
#include "material.h"
#include <vector>
#include <deque>

namespace MCPP {

using namespace std;

/**
 * @brief The Sample class describes a multi-layer sample made of infinitely
 * extended slabs
 */

class Sample : public BaseObject
{
public:
    Sample(BaseObject *parent=NULL);
    void addLayer(Material *material, double thickness);
    void addPreLayer(Material *material, double thickness);
    void setSurroundingEnvironment(Material *material);
    void setSurroundingEnvironment(Material *leftMaterial, Material *rightMaterial);
    unsigned int nLayers() const;
    const deque<MCfloat> *zBoundaries() const;
    Material *material(unsigned int layerIndex) const;
    unsigned int layerAt(MCfloat z);
    MCfloat totalThickness() const;

private:
    unsigned int _nLayers;
    deque<MCfloat> _zBoundaries;
    deque<Material*> materials;

    virtual void describe_impl() const;
};

}
#endif // SAMPLE_H
