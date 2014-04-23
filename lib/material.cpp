#include "material.h"
#include <cmath>
#include <limits>

using namespace std;

Material::Material()
{
    ls = numeric_limits<MCfloat>::infinity(); //non-scattering medium
    n = 1;
    v = LIGHT_SPEED;
}

/**
 * @brief Sets the refractive index and the speed of light in this medium
 * according to the given wavelength
 *
 * @param um wavelength in micrometers
 */

void Material::setWavelegth(MCfloat um) {
    n = dispersionRelation(um);
    v = LIGHT_SPEED/n;
}

/**
 * @brief Implements the dispersion relation
 * @param lambda_um wavelength in micrometers
 * @return the refractive index at the given wavelength
 *
 * The default material is dispersionless.
 */

MCfloat Material::dispersionRelation(MCfloat lambda_um) {
    return n;
}

/**
 * \todo implement http://emtoolbox.nist.gov/Wavelength/Edlen.asp formula with
 * predefined values and relative set methods
 */

Air::Air() :
    Material()
{
    n = 1.00027499;
}

// http://jartweb.net/blog/wp-content/uploads/2013/09/Chapter7.pdf
// http://www.humboldtcanada.com/presentations_air/jennifer.pdf
// Bonsch, Potulski, Metrologia 35 (1998) 133-139

// x is the CO2 concentration, by volume
// temperature in celsius
// f water vapour pressure in Pa
// P in Pa

/**
 * @brief FreeSpace::dispersionRelation
 * @param lambda_um
 * @return
 *
 * \todo implement dispersion relation
 */

MCfloat Air::dispersionRelation(MCfloat lambda_um) {
// TODO
    return n;
}


GlassSlide::GlassSlide() :
    Material()
{
    n = 1.5203;
}

MCfloat GlassSlide::dispersionRelation(MCfloat lambda_um) {
    MC_ASSERT_MSG(lambda_um > 0, "Invalid wavelength");
    return sqrt(1 + 1.282086558597*pow(lambda_um,2)/(pow(lambda_um,2) - 0.01023694363174)
                    + 0.05984826992046*pow(lambda_um,2)/(pow(lambda_um,2) - 12.09530215672));
}




NorlandOpticalAdhesive65::NorlandOpticalAdhesive65() :
    Material()
{
    n = 1.514;
}

/**
 * \copydoc Material::setWavelegth()
 *
 * Coefficients provided from <a href="http://www.thorlabs.de/newgrouppage9.cfm?objectgroup_id=196&pn=NOA65#196">Thorlabs</a>
 * (see under Optical Properties tab)
 */

MCfloat NorlandOpticalAdhesive65::dispersionRelation(MCfloat lambda_um) {
    MC_ASSERT_MSG(lambda_um > 0, "Invalid wavelength");
    return 1.50631 + 5.43562E-3/pow(lambda_um,2) + 27.7798E-6/pow(lambda_um,4);
}




Vacuum::Vacuum() :
    Material()
{
}
