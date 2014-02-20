#include "gaussianraybundlesource.h"

#include <boost/math/special_functions/erf.hpp>

using namespace boost::math::constants;
using namespace boost::math;

/**
 * @brief Constructs a cylindrically symmetric Gaussian ray-optics bundle
 * @param lensWaist spot size in \f$ z_\text{lens} = z_\text{waist} - d\f$
 * @param waist minimum spot size
 * @param lensDistance distance \f$ d \f$ between the beam waist and the
 *        focusing lens
 * @param parent
 */

GaussianRayBundleSource::GaussianRayBundleSource(double lensWaist, double waist, double lensDistance, BaseObject *parent) :
    Source(parent)
{
    xWaist=yWaist=waist;
    xLensWaist=yLensWaist=lensWaist;
    d=lensDistance;
    init();
}

/**
 * @brief Constructs a non-cylindrically symmetric Gaussian ray-optics bundle
 * @param lensXWaist spot size in \f$ z_\text{lens} = z_\text{waist} - d\f$
 *        along the \f$ x \f$ axis
 * @param lensYWaist spot size in \f$ z_\text{lens} = z_\text{waist} - d\f$
 *        along the \f$ y \f$ axis
 * @param xWaist minimum spot size along the \f$ x \f$ axis
 * @param yWaist minimum spot size along the \f$ y \f$ axis
 * @param lensDistance distance \f$ d \f$ between the beam waist and the
 *        focusing lens
 * @param parent
 *
 *
 * Non-cylindrically symmetric implementation
 */

GaussianRayBundleSource::GaussianRayBundleSource(double lensXWaist, double lensYWaist, double xWaist, double yWaist, double lensDistance, BaseObject *parent) :
    Source(parent)
{
    xLensWaist=lensXWaist;
    yLensWaist=lensYWaist;
    this->xWaist=xWaist;
    this->yWaist=yWaist;
    d=lensDistance;
    init();
}

void GaussianRayBundleSource::init()
{
    setZWaist(0);
    uRand = new UniformDistribution(0,1,this);
}

/**
 * @brief Sets the beam's waist position along \f$ z \f$
 * @param zWaist
 *
 *
 * This method allows to specify a different position on the \f$ z \f$-axis for
 * the waist of the beam. By default this position is set to coincide with \f$
 * z = 0 \f$, i.e. with the first interface of the first scattering layer of a
 * sample.
 *
 * This particular implementation assumes for simplicity that the spot size of
 * the simulated beam is known at \f$ z=0 \f$ (i.e. in its focused waist) and
 * at \f$ z=-d, d \gg z_\text{R} \f$ (i.e. on the plane of a focusing lens, on
 * which the beam is supposed to impinge almost collimated)
 */

void GaussianRayBundleSource::setZWaist(double value)
{
    zWaistReal = value;
    zWaistInEnvironment = value;
}

double GaussianRayBundleSource::zWaist()
{
    return zWaistReal;
}

/**
 * @brief Sets the lens position \f$ z_\text{lens} = z_\text{waist} - d\f$
 * @param value
 */

void GaussianRayBundleSource::setZLens(double value)
{
    zWaistInEnvironment = value + d;
    zWaistReal = zWaistInEnvironment;
}

double GaussianRayBundleSource::zLens() const
{
    return zWaistInEnvironment - d;
}

void GaussianRayBundleSource::focus(double zWaist, Sample *sample)
{
    int lensLayer, waistLayer;
    setZWaist(zWaist);

    waistLayer = sample->layerAt(zWaist);
    lensLayer = sample->layerAt(zLens());

    if(lensLayer == 0 && waistLayer == 0) {
        return;
    }

    zWaistInEnvironment = 0;
    const deque<double> *zBoundaries = sample->zBoundaries();

    double n0 = sample->material(0)->n;

    int i;
    for(i = 1; i <= waistLayer-1; i++) {
        double thickness = zBoundaries->at(i) - zBoundaries->at(i-1);
        double ni = sample->material(i)->n;
        zWaistInEnvironment += thickness * n0/ni;
    }
    double depthInLastLayer = zWaist - zBoundaries->at(waistLayer-1);
    double nLast = sample->material(waistLayer)->n;
    zWaistInEnvironment += depthInLastLayer * n0/nLast;
    zWaistInEnvironment += zBoundaries->at(0);
}

/**
 * \copydoc Source::constructWalker()
 * \pre walkTimeDistribution has to be valid
 */

void GaussianRayBundleSource::spinPosition(Walker *walker) const {
    walker->r0[0] = xLensWaist * one_div_root_two<double>() * erf_inv<double>(2*uRand->spinOpen() -1);
    walker->r0[1] = yLensWaist * one_div_root_two<double>() * erf_inv<double>(2*uRand->spinOpen() -1);
    walker->r0[2] = zLens();
}

/**
 * \copydoc Source::spinDirection()
 * \pre
 */

void GaussianRayBundleSource::spinDirection(Walker *walker) const {
    double xW = xWaist * one_div_root_two<double>() * erf_inv<double>(2*uRand->spinOpen() -1);
    double yW = yWaist * one_div_root_two<double>() * erf_inv<double>(2*uRand->spinOpen() -1);
    double connectingVector[3];
    connectingVector[0] = xW - walker->r0[0];
    connectingVector[1] = yW - walker->r0[1];
    connectingVector[2] = d;
    double connectingVectorNorm = sqrt(pow(connectingVector[0],2) + pow(connectingVector[1],2) + pow(connectingVector[2],2));
    for (int i = 0; i < 3; ++i) {
        walker->k0[i] = connectingVector[i]/connectingVectorNorm;
    }
}
