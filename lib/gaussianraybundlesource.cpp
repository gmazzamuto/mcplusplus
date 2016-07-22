/*
  This file is part of MCPlusPlus.

  MCPlusPlus is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Mcplusplus is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with MCPlusPlus.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <MCPlusPlus/gaussianraybundlesource.h>

#include <boost/math/special_functions/erf.hpp>

using namespace boost::math::constants;
using namespace boost::math;
using namespace MCPP;

/**
 * @brief Constructs a cylindrically symmetric Gaussian ray-optics bundle
 * @param lensWaist spot size in \f$ z_\text{lens} = z_\text{waist} - d\f$
 * @param waist minimum spot size
 * @param lensDistance distance \f$ d \f$ between the beam waist and the
 *        focusing lens
 * @param env the environment Material where the lens is placed (defaults to Vacuum())
 * @param parent
 *
 *
 * The lensDistance parameter fully characterizes the lens convergence power.
 * The two spot sizes refer to the beam size at the lens position and at the
 * waist.
 */

GaussianRayBundleSource::GaussianRayBundleSource(double lensWaist, double waist,
                                                 double lensDistance,
                                                 Material *env,
                                                 BaseObject *parent) :
    Source(parent)
{
    xWaist=yWaist=waist;
    xLensWaist=yLensWaist=lensWaist;
    d=lensDistance;
    environment = env;
    if(environment == NULL)
        environment = new Vacuum();
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
 * @param env the environment Material where the lens is placed (defaults to
 *        Vacuum())
 * @param parent
 *
 *
 * Non-cylindrically symmetric implementation
 */

GaussianRayBundleSource::GaussianRayBundleSource(double lensXWaist,
                                                 double lensYWaist,
                                                 double xWaist,
                                                 double yWaist,
                                                 double lensDistance,
                                                 Material *env,
                                                 BaseObject *parent) :
    Source(parent)
{
    xLensWaist=lensXWaist;
    yLensWaist=lensYWaist;
    this->xWaist=xWaist;
    this->yWaist=yWaist;
    d=lensDistance;
    environment = env;
    if(environment == NULL)
        environment = new Vacuum();
    init();
}

void GaussianRayBundleSource::init()
{
    setZWaist(0);
    uRand = new UniformDistribution(0,1,this);
    clearObjectsToCheck();
    objectsToCheck.push_back((const BaseObject**)&walkTimeDistribution);
}

/**
 * @brief Sets the beam's waist position along \f$ z \f$
 * @param zWaist
 *
 *
 * This method allows to specify a different position on the \f$ z \f$-axis for
 * the waist of the beam. By default this position is set to coincide with \f$
 * z = 0 \f$.
 *
 * This function will place the waist at the specified position assuming there
 * are no interfaces between the lens and the waist. If you want the waist to
 * really be at the specified position, accounting for the presence of possible
 * layers of different materials in between, use focus() instead.
 */

void GaussianRayBundleSource::setZWaist(double value)
{
    zWaistReal = value;
    zWaistInEnvironment = value;
    _z0 = zLens();
}

/**
 * @brief The actual waist \f$ z \f$ coordinate.
 * @return
 *
 *
 * In order to set the waist position you must use focus(). The value returned
 * makes sense only if the waist has been set explicitly via the focus()
 * function.
 */

MCfloat GaussianRayBundleSource::zWaist()
{
    return zWaistReal;
}

/**
 * @brief Sets the lens position \f$ z_\text{lens} = z_\text{waist} - d\f$
 * @param value
 *
 *
 * The default lens position is such that \f$ z_\text{waist}=0 \f$.
 */

void GaussianRayBundleSource::setZLens(double value)
{
    zWaistInEnvironment = value + d;
    zWaistReal = zWaistInEnvironment;
    _z0 = zLens();
}

void GaussianRayBundleSource::setWavelength(double um)
{
    Source::setWavelength(um);
    environment->setWavelength(um);
}

void GaussianRayBundleSource::spinTime(Walker *walker) const
{
    walker->walkTime = walkTimeDistribution->spin();
    MCfloat timeOffsetNoSample = d / environment->v;
    MCfloat s = d / walker->k0[2];
    walker->walkTime += (timeOffsetNoSample-s/environment->v);
}

MCfloat GaussianRayBundleSource::zLens() const
{
    return zWaistInEnvironment - d;
}

void GaussianRayBundleSource::describe_impl() const
{
    stringstream ss;
    ss << "xWaist = " << xWaist << " yWaist = " << yWaist << " xLensWaist = "
       << xLensWaist << " xLensWaist = " << xLensWaist << endl;
    logMessage(ss.str());
    ss.str(std::string());
    ss << "zWaistReal = " << zWaistReal << " zWaistInEnvironment = " <<
          zWaistInEnvironment << " d = " << d << endl;
    logMessage(ss.str());
    ss.str(std::string());
    ss << "zLens = " << zLens() << endl;
    logMessage(ss.str());
}

BaseObject *GaussianRayBundleSource::clone_impl() const
{
    GaussianRayBundleSource *src = new GaussianRayBundleSource(
                xLensWaist, yLensWaist, xWaist, yWaist, d);
    src->walkTimeDistribution = walkTimeDistribution;
    return src;
}

/**
 * @brief Moves the lens in order to have the waist at the specified position
 * @param zWaist \f$ z \f$ coordinate of the waist
 * @param sample Pointer to the sample
 * @return true on success, false if there's no solution with the lens
 *         positioned in the 0-th layer
 *
 * This function places the lens at the proper position in order for the beam
 * waist to physically occurr at the specified position, accounting for the
 * presence of the layers and pre-layers specified by sample. The lens is
 * supposed to be located in the 0-th layer (i.e. the surrounding environment).
 * The resulting lens position is computed with the following formula: \f$
 * z_\text{lens} = z_0 + \sum_i \frac{n_0}{n_i}L_i - d \f$ where \f$ z_0 \f$ is
 * the upper boundary of the 0-th layer, \f$ L_i \f$ is the thickness of the
 * \f$i\f$-th layer occurring between the lens and the waist position.
*/

bool GaussianRayBundleSource::focus(double zWaist, Sample *sample)
{
    int lensLayer, waistLayer;
    setZWaist(zWaist);

    waistLayer = sample->layerAt(zWaist);
    lensLayer = sample->layerAt(zLens());

    if(lensLayer == 0 && waistLayer == 0) {
        return true;
    }

    zWaistInEnvironment = 0;
    const deque<MCfloat> *zBoundaries = sample->zBoundaries();

    MCfloat n0 = sample->material(0)->n;

    int i;
    for(i = 1; i <= waistLayer-1; i++) {
        MCfloat thickness = zBoundaries->at(i) - zBoundaries->at(i-1);
        MCfloat ni = sample->material(i)->n;
        zWaistInEnvironment += thickness * n0/ni;
    }
    MCfloat depthInLastLayer = zWaist - zBoundaries->at(waistLayer-1);
    MCfloat nLast = sample->material(waistLayer)->n;
    zWaistInEnvironment += depthInLastLayer * n0/nLast;
    zWaistInEnvironment += zBoundaries->at(0);

    if(zLens() < zBoundaries->at(0))
        return true;
    else {
        setZWaist(0);
        return false;
    }
}

/**
 * \copydoc Source::constructWalker()
 * \pre walkTimeDistribution has to be valid
 *
 * The walker is created with \f$ r_0 \f$ on the lens.
 */

void GaussianRayBundleSource::spinPosition(Walker *walker) const {
    walker->r0[0] = xLensWaist * one_div_root_two<MCfloat>()
            * erf_inv<MCfloat>(2*uRand->spinOpen() -1);
    walker->r0[1] = yLensWaist * one_div_root_two<MCfloat>()
            * erf_inv<MCfloat>(2*uRand->spinOpen() -1);
    walker->r0[2] = zLens();
}

/**
 * \copydoc Source::spinDirection()
 * \pre
 */

void GaussianRayBundleSource::spinDirection(Walker *walker) const {
    MCfloat xW = xWaist * one_div_root_two<MCfloat>()
            * erf_inv<MCfloat>(2*uRand->spinOpen() -1);
    MCfloat yW = yWaist * one_div_root_two<MCfloat>()
            * erf_inv<MCfloat>(2*uRand->spinOpen() -1);
    MCfloat connectingVector[3];
    connectingVector[0] = xW - walker->r0[0];
    connectingVector[1] = yW - walker->r0[1];
    connectingVector[2] = d;
    MCfloat connectingVectorNorm = sqrt(pow(connectingVector[0],2)
            + pow(connectingVector[1],2) + pow(connectingVector[2],2));
    for (int i = 0; i < 3; ++i) {
        walker->k0[i] = connectingVector[i]/connectingVectorNorm;
    }
}
