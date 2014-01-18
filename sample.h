#ifndef SAMPLE_H
#define SAMPLE_H

#include "layer.h"

class Sample
{
public:
    Sample();
    void addLayer(Layer *layer);

protected:
    int nLayers;
};

#endif // SAMPLE_H
