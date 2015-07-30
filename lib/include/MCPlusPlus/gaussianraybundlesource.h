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

#ifndef GAUSSIANRAYBUNDLESOURCE_H
#define GAUSSIANRAYBUNDLESOURCE_H

#include "source.h"
#include "sample.h"

namespace MCPP {

/**
 * @brief Ray-optic description of a Gaussian beam waist
 *
 * Modeled after \cite milsom2000ray
 *
 * The intensity distribution in a Gaussian beam is represented by a bundle of
 * rays in which each ray has a random pointing error. The construction of the
 * bundle requires the knowledge of \f$ 1/\mathrm{e}^2 \f$ spot size at the
 * beam waist \f$ z_\text{w}\f$ and at a distance \f$ z_\text{lens} =
 * z_\text{w} -d, d \gg z_\text{R}\f$ where a lens is supposedly located. The
 * beauty of this approach is that it not only gets the intensity distribution
 * correct at these two extremes, it also gets it right at all other values of
 * \f$ z \f$ (provided that \f$ d \gg z_\text{R}\f$). This source constructs
 * each walker at the \f$ z_\text{lens} \f$ position.
 *
 * The lens can be positioned in space by calling either focus() or setZLens()
 * to specify the waist and the lens position respectively.
 *
 * Time distribution is left unspecified, and must be set serparately. The
 * specified time distribution is valid at the waist position, assuming
 * propagation from the lens to the waist occurring in the environment material
 * specified in the constructor.
 */

class GaussianRayBundleSource : public Source
{
public:
    GaussianRayBundleSource(double lensWaist, double waist, double lensDistance, Material *env=NULL, BaseObject *parent=NULL);
    GaussianRayBundleSource(double lensXWaist, double lensYWaist, double xWaist, double yWaist, double lensDistance, Material *env=NULL, BaseObject *parent=NULL);
    bool focus(double zWaistReal, Sample *sample);
    void setZLens(double value);
    virtual void setWavelength(double um);

protected:
    virtual void spinTime(Walker *walker) const;

private:
    void init();
    virtual void spinDirection(Walker *walker) const;
    virtual void spinPosition(Walker *walker) const;
    void setZWaist(double value);
    MCfloat zWaist();
    MCfloat zLens() const;
    virtual void describe_impl() const;
    virtual BaseObject *clone_impl() const;

    MCfloat xWaist, yWaist, xLensWaist, yLensWaist;
    MCfloat zWaistReal;
    MCfloat d;
    MCfloat zWaistInEnvironment;
    UniformDistribution *uRand;
    Material *environment;
};

}
#endif // GAUSSIANRAYBUNDLESOURCE_H
