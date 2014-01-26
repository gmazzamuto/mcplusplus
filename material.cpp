#include "material.h"

Material::Material()
{
    isScattering = true;
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


/**
 * \todo this is meant to reproduce the optical properties of the basic
 * glass slides from Carlo Erba. What kind of glass are they made of? Let's say
 * this one: http://refractiveindex.info/?group=GLASSES&material=SchottB270
 * but further checking is needed.
 */

GlassSlide::GlassSlide() :
    Material()
{
    isScattering = false;
    n = 1.5203;
}

/**
 * \todo implement http://www.thorlabs.de/newgrouppage9.cfm?objectgroup_id=196&pn=NOA65#196
 * dispersion formula (see under Optical Properties tab)
 */

NorlandOpticalAdhesive65::NorlandOpticalAdhesive65() :
    Material()
{
    isScattering = false;
    n = 1.514;
}
