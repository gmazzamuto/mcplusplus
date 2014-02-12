#include "sample.h"

Sample::Sample(BaseObject *parent) :
    BaseObject(parent)
{
    _nLayers=0;
    totThickness = 0;
    _zBoundaries.push_back(totThickness);
}

/**
 * @brief Adds a layer to the sample.
 * @param material
 * @param thickness
 *
 *
 * A layer is an infinitely extended slab with the given material and the given
 * finite thickness (measured along \f$ z \f$). Layer are stacked from left to
 * right. The first interface of the first layer is placed in \f$ z=0 \f$.
 */

void Sample::addLayer(const Material &material, double thickness) {
    _nLayers++;
    layers.push_back(material);
    totThickness += thickness;
    _zBoundaries.push_back(totThickness);
    materials.push_back(material);
}

void Sample::setSurroundingEnvironment(const Material &material) {
    materials.push_front(material);
    materials.push_back(material);
}

void Sample::setSurroundingEnvironment(const Material &frontMaterial, const Material &backMaterial) {
    materials.push_front(frontMaterial);
    materials.push_back(backMaterial);
}

double Sample::totalThickness() const {
    return totThickness;
}

int Sample::nLayers() const {
    return _nLayers;
}

const vector<double> *Sample::zBoundaries() const {
    return &_zBoundaries;
}

const Material *Sample::material(int layerIndex) const {
    return &materials.at(layerIndex);
}
