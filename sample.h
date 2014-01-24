#ifndef SAMPLE_H
#define SAMPLE_H

#include <baseobject.h>
#include "material.h"
#include <vector>

using namespace std;

class Sample : public BaseObject
{
public:
    Sample();
    void addLayer(const Material *material, double thickness);
    void setSurroundingEnvironment(const Material *material);
    double totalThickness() {return totThickness;}

private:
    int nLayers;
    vector<Material> layers;
    vector<double> zBoundaries;
    double totThickness;
    Material environment;
};

#endif // SAMPLE_H
