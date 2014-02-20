#ifndef GAUSSIANRAYBUNDLESOURCE_H
#define GAUSSIANRAYBUNDLESOURCE_H

#include "source.h"
#include "sample.h"

/**
 * @brief Ray-optic description of a Gaussian beam waist
 *
 * Modeled after Milsom, P.K. "A ray-optic, Monte Carlo, description of a
 * Gaussian beam waist - applied to reverse saturable absorption"
 * <em>Appl. Phys. B</em>, 70(4), 593-599 (April 2000).
 *
 * The intensity distribution in a Gaussian beam is represented by a bundle of
 * rays in which each ray has a random pointing error. The construction of the
 * bundle requires the knowledge of \f$ 1/\mathrm{e}^2 \f$ spot size at the
 * beam waist and at a distance \f$ z = z_\text{w} -d, d \gg z_\text{R}\f$. The
 * beauty of this approach is that it not only gets the intensity distribution
 * correct at these two extremes, it also gets it right at all other values of
 * \f$ z \f$ (provided that \f$ d \gg z_\text{R}\f$). This source attempts to
 * construct each walker in the beam's waist, but being by default a beam-like
 * source (see Source::isBeamLike()) it will perform a check on the walker's
 * previous history if the beam waist happens to be inside some layer.
 *
 * Time distribution is left unspecified, and must be set serparately.
 */

class GaussianRayBundleSource : public Source
{
public:
    GaussianRayBundleSource(double lensWaist, double waist, double lensDistance, BaseObject *parent=NULL);
    GaussianRayBundleSource(double collimatedXWaist, double collimatedYWaist, double focusedXWaist, double focusedYWaist, double lensDistance, BaseObject *parent=NULL);
    void setZWaist(double value);
    double zWaist();
    void setZLens(double value);
    double zLens() const;
    void focus(double zWaistReal, Sample *sample);

private:
    void init();
    virtual void spinDirection(Walker *walker) const;
    virtual void spinPosition(Walker *walker) const;

    double xWaist, yWaist, xLensWaist, yLensWaist;
    double zWaistReal;
    double d;
    double zWaistInEnvironment;
    UniformDistribution *uRand;
};

#endif // GAUSSIANRAYBUNDLESOURCE_H
