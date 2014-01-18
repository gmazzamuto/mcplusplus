#include "sample.h"

Sample::Sample()
{
    nLayers=0;
}

Sample::addLayer(Layer *layer) {
    nLayers++;
}
