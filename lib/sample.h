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
    Sample(BaseObject *parent=NULL);
    void addLayer(const Material &material, double thickness);
    void setSurroundingEnvironment(const Material &material);
    void setSurroundingEnvironment(const Material &frontMaterial, const Material &backMaterial);
    double totalThickness() const;
    int nLayers() const;
    const vector<double> * zBoundaries() const;
    const Material *material(int layerIndex) const;

private:
    int _nLayers;
    vector<Material> layers;
    vector<double> _zBoundaries;
    deque<Material> materials;
    double totThickness;
};

#endif // SAMPLE_H
