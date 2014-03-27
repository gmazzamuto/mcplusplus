#include "sample.h"

Sample::Sample(BaseObject *parent) :
    BaseObject(parent)
{
    _nLayers=0;
    _zBoundaries.push_back(0);

    materials.push_front(Vacuum());
    materials.push_back(Vacuum());
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
    materials.insert(materials.end()-1,material);
}

/**
 * @brief Adds a \em non-scattering pre-layer to the sample
 * @param material
 * @param thickness
 *
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
    materials.insert(materials.begin()+1,material);
}

void Sample::setSurroundingEnvironment(const Material &material) {
    setSurroundingEnvironment(material, material);
}

void Sample::setSurroundingEnvironment(const Material &frontMaterial, const Material &backMaterial) {
    materials.pop_back();
    materials.pop_front();
    materials.push_front(frontMaterial);
    materials.push_back(backMaterial);
}

int Sample::nLayers() const {
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
 *
 */

const deque<double> *Sample::zBoundaries() const {
    return &_zBoundaries;
}

const Material *Sample::material(int layerIndex) const {
    return &materials.at(layerIndex);
}

int Sample::layerAt(double z)
{
    int n = _zBoundaries.size();
    int i;
    for(i = 0; i < n; i++) {
        if(z <= _zBoundaries[i])
            break;
    }
    return i;
}
