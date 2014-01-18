#ifndef LAYER_H
#define LAYER_H

/**
 * @brief Base Layer class.
 *
 * A layer is an infinitely extended slab of finite or semi-infinite thickness
 * (measured along \f$ z \f$). The simplest possible finite slab sample is made
 * of 3 layers: the sample layer itself plus one semi-infinte environment layer
 * for each side.
 */

class Layer
{
public:
    Layer();
    int layerIndex;
    bool isPartOfTheSample;
    double thickness;
    double ls;
    double g;
    double lt;
    double refractiveIndex;
    double zStart, zEnd;
};


class FreeSpace : public Layer
{
public:
    FreeSpace();
};

#endif // LAYER_H
