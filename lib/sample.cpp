#include "sample.h"

Sample::Sample(BaseObject *parent) :
    BaseObject(parent)
{
    _nLayers=0;
    _zBoundaries.push_back(0);
}

/**
 * @brief Adds a layer to the sample.
 * @param material
 * @param thickness
 *
 *
 * A layer is an infinitely extended slab with the given material and the given
 * finite thickness (measured along \f$ z \f$). Layers are stacked from left to
 * right. The first interface of the first \em scattering layer is placed in
 * \f$ z=0 \f$.
 */

void Sample::addLayer(const Material &material, double thickness) {
    _nLayers++;
    layers.push_back(material);
    _zBoundaries.push_back(_zBoundaries.back() + thickness);
    if(materials.size() > _nLayers) // then it means that someone already set the environment!
        materials.insert(materials.end()-1,material);
    materials.push_back(material);
}

/**
 * @brief Adds a \em non-scattering pre-layer to the sample
 * @param material
 * @param thickness
 *
 * A pre-layer, if present, is an infinitely extended slab with the given
 * material and the given finite thickness (measured along \f$ z \f$). In
 * contrast with ordinary layers, a pre-layer (usually representing a sample
 * holder/container wall) must be a \em non-scattering material and will be
 * stacked in the negative \f$ z \f$ direction (i.e. pre-layers are stacked
 * from left to right).
 */

void Sample::addPreLayer(const Material &material, double thickness)
{
    _nLayers++;
    layers.push_front(material);
    _zBoundaries.push_front(_zBoundaries.front() - thickness);
    if(materials.size() > _nLayers) // then it means that someone already set the environment!
        materials.insert(materials.begin()+1,material);
    materials.push_front(material);
}

void Sample::setSurroundingEnvironment(const Material &material) {
    materials.push_front(material);
    materials.push_back(material);
}

void Sample::setSurroundingEnvironment(const Material &frontMaterial, const Material &backMaterial) {
    materials.push_front(frontMaterial);
    materials.push_back(backMaterial);
}

int Sample::nLayers() const {
    return _nLayers;
}

const deque<double> *Sample::zBoundaries() const {
    return &_zBoundaries;
}

const Material *Sample::material(int layerIndex) const {
    return &materials.at(layerIndex);
}
