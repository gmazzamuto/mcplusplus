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

#include <MCPlusPlus/sample.h>

#include <limits>

using namespace MCPP;

Sample::Sample(BaseObject *parent) :
    BaseObject(parent)
{
    _nLayers=0;
    _zBoundaries.push_back(0);

    materials.push_front(new Vacuum());
    materials.push_back(new Vacuum());
}

/**
 * @brief Adds a layer to the sample.
 * @param material
 * @param thickness
 *
 *
 * A layer is an infinitely extended slab with the given material and the given
 * finite thickness (measured along \f$ z \f$). Layers are stacked from left to
 * right. The first interface of the first layer is placed in
 * \f$ z=0 \f$.
 */

void Sample::addLayer(Material *material, double thickness) {
    _nLayers++;
    _zBoundaries.push_back(_zBoundaries.back() + thickness);
    materials.insert(materials.end() - 1, material);
}

/**
 * @brief Adds a \em non-scattering pre-layer to the sample
 * @param material
 * @param thickness
 *
 *
 * Pre-layers are stacked from left to right. The first interface of the first
 * layer is placed in \f$ z=0 \f$.
 */

void Sample::addPreLayer(Material *material, double thickness)
{
    _nLayers++;
    _zBoundaries.push_front(_zBoundaries.front() - thickness);
    materials.insert(materials.begin() + 1,material);
}

/**
 * @brief Set the material that surrounds the sample
 * @param material
 */

void Sample::setSurroundingEnvironment(Material *material) {
    setSurroundingEnvironment(material, material);
}

/**
 * @brief Set the materials surrounding the sample on the two sides (left and
 *        right)
 * @param leftMaterial
 * @param rightMaterial
 */

void Sample::setSurroundingEnvironment(Material *leftMaterial,
                                       Material *rightMaterial) {
    materials.pop_back();
    materials.pop_front();
    materials.push_front(leftMaterial);
    materials.push_back(rightMaterial);
}

/**
 * @brief The total number of layers and pre-layers that make up the sample
 * @return
 */

unsigned int Sample::nLayers() const {
    return _nLayers;
}

/**
 * @brief Returns a pointer to a deque containing the layer boundaries along
 *        \f$ z \f$
 * @return
 *
 *
 * The n-th element of this deque is the upper boundary of the n-th layer. The
 * 0-th element is the upper boundary of the surrounding environment
 * semi-infinte space towards negative infinity.
 */

const deque<MCfloat> *Sample::zBoundaries() const {
    return &_zBoundaries;
}

/**
 * @brief The material of the \f$ i \f$-th layer
 * @param layerIndex
 * @return
 */

Material *Sample::material(unsigned int layerIndex) const {
    return materials.at(layerIndex);
}

/**
 * @brief The index of the layer in which the given \f$ z \f$ lies
 * @param z
 * @return
 *
 * Interfaces are considered to belong to the leftmost layer
 */

unsigned int Sample::layerAt(MCfloat z)
{
    int n = _zBoundaries.size();
    int i;
    for(i = 0; i < n; i++) {
        if(z <= _zBoundaries[i])
            break;
    }
    return i;
}

MCfloat Sample::totalThickness() const
{
    return _zBoundaries.at(_zBoundaries.size()-1);
}

void Sample::describe_impl() const
{
    for (uint i = 0; i < materials.size(); ++i) {
        stringstream ss;
        const Material *mat = materials.at(i);
        ss << "Layer  " << i;
        if(i > 0 && i <= _nLayers)
            ss << " thickness = " <<
                  _zBoundaries.at(i) - _zBoundaries.at(i-1) << "\t";
        else
            ss << " thickness = infinity";
        ss << "\tMaterial: n = " << mat->n << "\tv = " << mat->v;
        if(mat->ls != numeric_limits<MCfloat>::infinity())
             ss << "\tls = " << mat->ls << "\tg = " << mat->g;
        logMessage(ss.str());
    }
}
