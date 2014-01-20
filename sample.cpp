#include "sample.h"

Sample::Sample()
{
    nLayers=0;
    totalThickness = 0;
}

void Sample::addLayer(const Material &material, double thickness) {
    layers.push_back(material);
    zBoundaries.push_back(totalThickness + thickness);
    totalThickness += thickness;
    nLayers++;
}
