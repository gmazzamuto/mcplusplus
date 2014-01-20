#include "sample.h"

Sample::Sample()
{
    nLayers=0;
    totThickness = 0;
}

/**
 * @brief Adds a layer to the sample.
 * @param material
 * @param thickness
 *
 *
 * A layer is an infinitely extended slab with the given material and the given
 * finite thickness (measured along \f$ z \f$).
 */

void Sample::addLayer(const Material *material, double thickness) {
    Material mat;
    mat = *material;
    layers.push_back(mat);
    totThickness += thickness;
    zBoundaries.push_back(totThickness);
    nLayers++;
}

void Sample::setSurroundingEnvironment(const Material *material) {
    environment = *material;
}
