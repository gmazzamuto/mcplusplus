#ifndef SAMPLE_H
#define SAMPLE_H

#include "material.h"
#include <vector>

using namespace std;

class Sample
{
public:
    Sample();
    void addLayer(const Material &material, double thickness);

private:
    int nLayers;
    vector<Material> layers;
    vector<double> zBoundaries;
    double totalThickness;
};

#endif // SAMPLE_H
