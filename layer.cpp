#include "layer.h"

Layer::Layer()
{
}

/**
 * \todo implement http://emtoolbox.nist.gov/Wavelength/Edlen.asp formula with predefined values and relative set methods
 */

FreeSpace::FreeSpace() :
    Layer()
{
    isPartOfTheSample = false;
    ls=g=0;
    refractiveIndex = 1.00027499; //we should try to implement http://emtoolbox.nist.gov/Wavelength/Edlen.asp and set this value automatically
}
