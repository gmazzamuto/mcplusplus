#include "source.h"
#include "costhetagenerator.h"
#include "psigenerator.h"
#include "math.h"
#include <boost/math/special_functions/erf.hpp>

using namespace boost::math::constants;
using namespace boost::math;

Source::Source(BaseObject *parent) :
    BaseRandom(parent)
{
    walkTimeDistribution = NULL;
    cosThetaDistribution = NULL;
    psiDistribution = NULL;
    for (int i = 0; i < 3; ++i) {
        r0Distribution[i] = NULL;
    }
}

void Source::spinDirection(Walker *walker) const {
    double cosTheta = cosThetaDistribution->spin();
    double cosPsi = cos(psiDistribution->spin());
    double sinTheta = sqrt(1 - pow(cosTheta,2));
    double sinPsi = sin(psiDistribution->spin());

    walker->k0[0] = sinTheta*cosPsi;
    walker->k0[1] = sinTheta*sinPsi;
    walker->k0[2] = cosTheta;
}

void Source::spinPosition(Walker *walker) const {
    for (int i = 0; i < 3; ++i) {
        walker->r0[i] = r0Distribution[i]->spin();
    }
}

void Source::spinTime(Walker *walker) const {
    walker->walkTime = walkTimeDistribution->spin();    // overwrapping?
}

/**
 * @brief Constructs a new walker.
 * @return A pointer to the newly constructed walker.
 */

Walker* Source::constructWalker() const {
    Walker *walker = new Walker();

    spinDirection(walker);
    spinPosition(walker);
    spinTime(walker);

    return walker;
}

void Source::setr0Distribution(AbstractDistribution **distrArray) {
    for (int i = 0; i < 3; ++i) {
        if(r0Distribution[i] != NULL)
            delete r0Distribution[i];
        r0Distribution[i] = distrArray[i];
        r0Distribution[i]->setParent(this);
    }
}

void Source::setk0Distribution(AbstractDistribution *cosThetaDistr, AbstractDistribution *psiDistr) {
    if(cosThetaDistribution != NULL)
        delete cosThetaDistribution;
    cosThetaDistribution = cosThetaDistr;
    cosThetaDistribution->setParent(this);
    if(psiDistribution != NULL)
        delete psiDistribution;
    psiDistribution = psiDistr;
    psiDistribution->setParent(this);
}

void Source::setWalkTimeDistribution(AbstractDistribution *distr) {
    walkTimeDistribution = distr;
    walkTimeDistribution->setParent(this);
}

void Source::setWavelength(double nm) {
    wl = nm;
}

double Source::wavelength() const {
    return wl;
}




PencilBeamSource::PencilBeamSource(BaseObject *parent) :
    Source(parent)
{

}

Walker* PencilBeamSource::constructWalker() const {
    Walker *walker = new Walker();
    walker->k0[2] = 1;

    return walker;
}




/**
 * @brief Constructs a cylindrically symmetric gaussian 2D intensity profile in
 *        the \f$ z=0\f$ plane
 * @param FWHM full width half maximum of the profile
 * @param parent
 */

GaussianBeamSource::GaussianBeamSource(double FWHM, BaseObject *parent) :
    Source(parent)
{
    NormalDistribution *distr = new NormalDistribution(0,FWHM, this);
    for (int i = 0; i < 2; ++i) {
        r0Distribution[i] = distr;
    }
    r0Distribution[2] = new DeltaDistribution(0, this);
}

/**
 * @brief Constructs a non-cylindrically symmetric gaussian 2D intensity profile
 *        in the \f$ z=0\f$ plane
 * @param xFWHM full width half maximum of the profile along the \f$ x \f$ axis
 * @param yFWHM full width half maximum of the profile along the \f$ y \f$ axis
 * @param parent
 */

GaussianBeamSource::GaussianBeamSource(double xFWHM, double yFWHM, BaseObject *parent) :
    Source(parent)
{
    r0Distribution[0] = new NormalDistribution(0,xFWHM, this);
    r0Distribution[1] = new NormalDistribution(0,yFWHM, this);
    r0Distribution[2] = new DeltaDistribution(0, this);
}

/**
 * \copydoc Source::constructWalker()
 * \pre walkTimeDistribution has to be valid
 */

Walker* GaussianBeamSource::constructWalker() const {
    Walker *walker;
    if(cosThetaDistribution != NULL || psiDistribution != NULL) //check if one of the k0Distributions has been overridden
        walker = Source::constructWalker(); //r0Distributions should already be set by now
    else {
        walker = new Walker();
        walker->k0[2] = 1;

        spinTime(walker);
    }
    return walker;
}




/**
 * @brief Constructs a cylindrically symmetric Gaussian ray-optics bundle
 * @param collimatedWaist spot size in \f$ z = -d = -z_\text{L}\f$ where the
 *        focusing lens is placed
 * @param focusedWaist minimum spot size (occurring at \f$ z = 0 \f$)
 * @param lensDistance distance \f$ d = z_\text{L} > 0\f$ between the beam waist
 *        and the focusing lens
 * @param parent
 *
 *
 * This particular implementation assumes for simplicity that the spot size of
 * the simulated beam is known at \f$ z=0 \f$ (i.e. in its focused waist) and
 * at \f$ z=-d, d \gg z_\text{R} \f$ (i.e. on the plane of a focusing lens, on
 * which the beam is supposed to impinge almost collimated)
 */

GaussianRayBundleSource::GaussianRayBundleSource(double collimatedWaist, double focusedWaist, double lensDistance, BaseObject *parent) :
    Source(parent)
{
    xFoc=yFoc=collimatedWaist;
    xColl=yColl=focusedWaist;
    d=lensDistance;
    uRand = new UniformDistribution(0,1,this);
}

/**
 * @brief Constructs a non-cylindrically symmetric Gaussian ray-optics bundle
 * @param collimateXdWaist spot size in \f$ z = -d = -z_\text{L}\f$ along the
 *        \f$ x \f$ axis
 * @param collimateYdWaist spot size in \f$ z = -d = -z_\text{L}\f$ along the
 *        \f$ y \f$ axis
 * @param focusedXWaist minimum spot size (occurring at \f$ z = 0 \f$) along the
 *        \f$ x \f$ axis
 * @param focusedYWaist minimum spot size (occurring at \f$ z = 0 \f$) along the
 *        \f$ y \f$ axis
 * @param lensDistance distance \f$ d = z_\text{L} > 0\f$ between the beam waist
 *        and the focusing lens
 * @param parent
 *
 *
 * Non-cylindrically symmetric implementation
 */

GaussianRayBundleSource::GaussianRayBundleSource(double collimatedXWaist, double collimatedYWaist, double focusedXWaist, double focusedYWaist, double lensDistance, BaseObject *parent) :
    Source(parent)
{
    xFoc=collimatedXWaist;
    yFoc=collimatedYWaist;
    xColl=focusedXWaist;
    yColl=focusedYWaist;
    d=lensDistance;
    uRand = new UniformDistribution(0,1,this);
}

/**
 * \copydoc Source::constructWalker()
 * \pre walkTimeDistribution has to be valid
 */

Walker* GaussianRayBundleSource::constructWalker() const {
    Walker *walker = new Walker();
    double rand[4];
    for (int i = 0; i < 4; ++i) {
        rand[i] = uRand->spinOpen();
    }
    walker->r0[0] = xColl * one_div_root_two<double>() * erf_inv<double>(2*rand[0] -1);
    walker->r0[1] = yColl * one_div_root_two<double>() * erf_inv<double>(2*rand[1] -1);
    double xL = xFoc * one_div_root_two<double>() * erf_inv<double>(2*rand[2] -1);
    double yL = yFoc * one_div_root_two<double>() * erf_inv<double>(2*rand[3] -1);
    double connectingVector[3];
    connectingVector[0] = walker->r0[0] - xL;
    connectingVector[1] = walker->r0[1] - yL;
    connectingVector[2] = d;
    double connectingVectorNorm = sqrt(pow(connectingVector[0],2) + pow(connectingVector[1],2) + pow(connectingVector[2],2));
    for (int i = 0; i < 3; ++i) {
        walker->k0[i] = connectingVector[i]/connectingVectorNorm;
    }

    spinTime(walker);

    return walker;
}




IsotropicPointSource::IsotropicPointSource(double z0, BaseObject *parent) :
    Source(parent)
{
    depth = z0;
    cosThetaDistribution = new IsotropicCosThetaGenerator;
    psiDistribution = new IsotropicPsiGenerator;
}

Walker* IsotropicPointSource::constructWalker() const {
    Walker *walker = new Walker();
    spinDirection(walker);
    walker->r0[2] = depth;

    return walker;
}
