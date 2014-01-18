#include "sample.h"

Sample::Sample()
{
    nLayers=0;
}

void Sample::addLayer(Layer *layer) {
    nLayers++;
}
