#ifndef SAMPLE_H
#define SAMPLE_H

#include "material.h"
#include <vector>

using namespace std;

class Sample
{
public:
    Sample();
    void addLayer(const Material &material);

protected:
    int nLayers;
    vector<Material> layers;
};

#endif // SAMPLE_H
