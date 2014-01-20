#include "sample.h"

Sample::Sample()
{
    nLayers=0;
}

void Sample::addLayer(const Material &material) {
    nLayers++;
    layers.push_back(material);
}
