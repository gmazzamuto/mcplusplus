#ifndef SAMPLE_H
#define SAMPLE_H

#include <baseobject.h>
#include "material.h"
#include <vector>
#include <deque>

using namespace std;

class Sample : public BaseObject
{
public:
    Sample();
    void addLayer(const Material &material, double thickness);
    void setSurroundingEnvironment(const Material &material);
    double totalThickness() const;
    int nLayers() const;
    vector<double> *zBoundaries();
    Material *material(int layerIndex);

private:
    int _nLayers;
    vector<Material> layers;
    vector<double> _zBoundaries;
    deque<Material> materials;
    double totThickness;
    Material environment;
};

#endif // SAMPLE_H
