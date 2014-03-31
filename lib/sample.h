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
    void addLayer(const Material &material, MCfloat thickness);
    void addPreLayer(const Material &material, MCfloat thickness);
    void setSurroundingEnvironment(const Material &material);
    void setSurroundingEnvironment(const Material &frontMaterial, const Material &backMaterial);
    int nLayers() const;
    const deque<MCfloat> *zBoundaries() const;
    const Material *material(int layerIndex) const;
    int layerAt(MCfloat z);

private:
    int _nLayers;
    deque<Material> layers;
    deque<MCfloat> _zBoundaries;
    deque<Material> materials;
};

#endif // SAMPLE_H
