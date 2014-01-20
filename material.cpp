#include "material.h"

Material::Material()
{
}

/**
 * \todo implement http://emtoolbox.nist.gov/Wavelength/Edlen.asp formula with predefined values and relative set methods
 */

FreeSpace::FreeSpace() :
    Material()
{
    ls=g=0;
    n = 1.00027499; //we should try to implement http://emtoolbox.nist.gov/Wavelength/Edlen.asp and set this value automatically
}
