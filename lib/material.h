#ifndef MATERIAL_H
#define MATERIAL_H

#include "MCglobal.h"

namespace MCPP {

const MCfloat LIGHT_SPEED = 299.792458; // um/ps
//const MCfloat INV_LIGHT_SPEED = 1.0/LIGHT_SPEED;

/**
 * @brief The Material class describes a material
 *
 * Specialized implementations should reimplement dispersionRelation()
 */

class Material
{
public:
    Material();
    virtual ~Material();
    MCfloat n;  /**< @brief refractive index (\f$ n \in \mathbb{R} \f$)*/
    MCfloat ls;  /**< @brief scattering mean free path*/
    MCfloat g;  /**< @brief scattering anisotropy factor \f$ g = \left\langle \cos \theta \right \rangle \f$*/
    MCfloat v;  /**< @brief phase velocity \note please beware of the difference between phase, group and energy velocity */

    void setWavelength(MCfloat um);

private:
    virtual MCfloat dispersionRelation(MCfloat lambda_um);
};




/**
 * @brief The Air class
 *
 * @todo parse Pressure, Temperature and RH?
 */

class Air : public Material
{
public:
    Air();
    virtual ~Air();

    MCfloat Pressure; /**< @brief air pressure (Pa), default is 101400 Pa*/
    MCfloat Temperature; /**< @brief air temperature (°C), default is 20°C */
    MCfloat RH; /**< @brief relative humidity (percent), default is 30% */

private:
    virtual MCfloat dispersionRelation(MCfloat lambda_um);
    MCfloat saturatedVapourPressure();
};




/**
 * @brief Empty space
 */

class Vacuum : public Material {

public:
    Vacuum();
    virtual ~Vacuum();
};




/**
 * @brief Glass microscope slide.
 *
 * This Material aims to reproduce the optical properties of <a
 * href="http://www.carloerbareagents.com/LabwareGruppoProdotto.aspx?id=22OB02AA&browsing=1">Forlab
 * Microscope Slides</a> purchased by Carlo Erba. Their catalogue page reads
 * "extrawhite soda lime glass" and such slides are commonly known to have a
 * refractive index of approximately 1.52@810nm (citation needed). In order to
 * implement a usable dispersion relation we referred to the Schott B270
 * Superwhite soda lime glass, a very common multi-purpose kind of glass that
 * happens to have the expected refractive index. Sadly enough, no empirical
 * dispersion relation for this yet common kind of glass happens to be known as
 * well. On page 9 of <a
 * href="http://www.reichmann-feinoptik.com/assets/applets/B_270_Superwite_e.pdf">this</a>
 * scanned specs datasheet a graph is shown with several values of the
 * refractive index. Based on these extrapolated values it was possible to
 * determine the appropriate <a
 * href="http://en.wikipedia.org/wiki/Sellmeier_equation">Sellmeier
 * coefficients</a> for the empirical dispersion relation.
 */

class GlassSlide : public Material
{
public:
    GlassSlide();
    virtual ~GlassSlide();

private:
    virtual MCfloat dispersionRelation(MCfloat lambda_um);
};



/**
 * @brief Norland Optical Adhesive 65
 *
 * This material reproduces a UV-curing optical adhesive produced by Norland.
 * Its dispersion relation has been implemented after the <a
 * href="http://en.wikipedia.org/wiki/Sellmeier_equation">Sellmeier
 * coefficients</a> as they have been measured by Thorlabs (see their <a
 * href="http://www.thorlabs.de/newgrouppage9.cfm?objectgroup_id=196&pn=NOA65">
 * Optical Properties tab</a> for further information).
 */

class NorlandOpticalAdhesive65 : public Material
{
public:
    NorlandOpticalAdhesive65();
    virtual ~NorlandOpticalAdhesive65();

private:
    virtual MCfloat dispersionRelation(MCfloat lambda_um);
};

}
#endif // MATERIAL_H
