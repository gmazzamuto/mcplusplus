#ifndef SAMPLE_H
#define SAMPLE_H

#include "baseobject.h"
#include "material.h"
#include <vector>
#include <deque>

namespace MCPP {

using namespace std;

/**
 * @brief The Sample class describes a multi-layer sample made of infinitely
 * extended slabs
 */

class Sample : public BaseObject
{
public:
    Sample(BaseObject *parent=NULL);
    void addLayer(Material *material, double thickness);
    void addPreLayer(Material *material, double thickness);
    void setSurroundingEnvironment(Material *material);
    void setSurroundingEnvironment(Material *leftMaterial, Material *rightMaterial);
    unsigned int nLayers() const;
    const deque<MCfloat> *zBoundaries() const;
    Material *material(unsigned int layerIndex) const;
    unsigned int layerAt(MCfloat z);
    MCfloat totalThickness() const;

private:
    unsigned int _nLayers;
    deque<MCfloat> _zBoundaries;
    deque<Material*> materials;

    virtual void describe_impl() const;
};

}
#endif // SAMPLE_H
