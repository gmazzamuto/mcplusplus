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
    void addLayer(Material *material, MCfloat thickness);
    void addPreLayer(Material *material, MCfloat thickness);
    void setSurroundingEnvironment(Material *material);
    void setSurroundingEnvironment(Material *frontMaterial, Material *backMaterial);
    unsigned int nLayers() const;
    const deque<MCfloat> *zBoundaries() const;
    Material *material(unsigned int layerIndex) const;
    unsigned int layerAt(MCfloat z);

private:
    unsigned int _nLayers;
    deque<MCfloat> _zBoundaries;
    deque<Material*> materials;

    virtual void describe_impl() const;
};

#endif // SAMPLE_H
