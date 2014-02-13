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
    void addPreLayer(const Material &material, double thickness);
    void setSurroundingEnvironment(const Material &material);
    void setSurroundingEnvironment(const Material &frontMaterial, const Material &backMaterial);
    int nLayers() const;
    const deque<double> *zBoundaries() const;
    const Material *material(int layerIndex) const;

private:
    int _nLayers;
    deque<Material> layers;
    deque<double> _zBoundaries;
    deque<Material> materials;
};

#endif // SAMPLE_H
