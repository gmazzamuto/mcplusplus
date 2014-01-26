#include "sample.h"

Sample::Sample()
{
    _nLayers=0;
    totThickness = 0;
    zBoundaries.push_back(totThickness);
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

void Sample::addLayer(const Material *material, double thickness) {
    Material mat;
    mat = *material;
    layers.push_back(mat);
    totThickness += thickness;
    zBoundaries.push_back(totThickness);
    _nLayers++;
}

void Sample::setSurroundingEnvironment(const Material *material) {
    environment = *material;
}

double Sample::totalThickness() {
    return totThickness;
}

int Sample::nLayers() {
    return _nLayers;
}
