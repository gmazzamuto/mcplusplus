#include "material.h"
#include <math.h>

Material::Material()
{
    isScattering = true;
}

/**
 * @brief Material::setWavelegth
 * @param um
 */

void Material::setWavelegth(double um) {

}

/**
 * \todo implement http://emtoolbox.nist.gov/Wavelength/Edlen.asp formula with
 * predefined values and relative set methods
 */

FreeSpace::FreeSpace() :
    Material()
{
    isScattering = false;
    n = 1.00027499;
}

// http://jartweb.net/blog/wp-content/uploads/2013/09/Chapter7.pdf
// http://www.humboldtcanada.com/presentations_air/jennifer.pdf
// Bonsch, Potulski, Metrologia 35 (1998) 133-139

// x is the CO2 concentration, by volume
// temperature in celsius
// f water vapour pressure in Pa
// P in Pa

void FreeSpace::setWavelegth(double um) {

}


GlassSlide::GlassSlide() :
    Material()
{
    isScattering = false;
    n = 1.5203;
}

void GlassSlide::setWavelegth(double um) {
    n = sqrt(1 + 1.282086558597*pow(um,2)/(pow(um,2) - 0.01023694363174) + 0.05984826992046*pow(um,2)/(pow(um,2) - 12.09530215672));
}




NorlandOpticalAdhesive65::NorlandOpticalAdhesive65() :
    Material()
{
    isScattering = false;
    n = 1.514;
}

/**
 * \copydoc Material::setWavelegth()
 *
 * Coefficients provided from <a href="http://www.thorlabs.de/newgrouppage9.cfm?objectgroup_id=196&pn=NOA65#196">Thorlabs</a>
 * (see under Optical Properties tab)
 */

void NorlandOpticalAdhesive65::setWavelegth(double um) {
    n = 1.50631 + 5.43562E-3/pow(um,2) + 27.7798E-6/pow(um,4);
}

