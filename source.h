#ifndef SOURCE_H
#define SOURCE_H

#include "walker.h"
#include "distributions.h"

/**
 * @brief Base Source class: constructs and initializes a Walker according to
 * given source term modeling.
 */

class Source : public BaseRandom
{
public:
    Source(BaseObject *parent=NULL);
    virtual Walker *constructWalker() const;
    void setr0Distribution(AbstractDistribution **distrArray);
    void setk0Distribution(AbstractDistribution *cosThetaDistr, AbstractDistribution *psiDistr);
    void setWalkTimeDistribution(AbstractDistribution *distr);
    void setWavelength(double nm);
    double wavelength() const;

protected:
    AbstractDistribution *r0Distribution[3];
    AbstractDistribution *cosThetaDistribution;
    AbstractDistribution *psiDistribution;
    AbstractDistribution *walkTimeDistribution;
    virtual void spinDirection(Walker *walker) const;
    virtual void spinPosition(Walker *walker) const;
    virtual void spinTime(Walker *walker) const;

private:
    double wl;
};




/**
 * @brief Pencil beam source \f$ \delta(\bm{r}) \, \delta (t) \, \delta
 * (\bm{k}_z) \f$, with \f$ \bm{k}_z = (0,0,1) \f$.
 */

class PencilBeamSource : public Source
{
public:
    PencilBeamSource(BaseObject *parent=NULL);
    Walker *constructWalker() const;
};




/**
 * @brief 2-dimensional \f$ xy \f$ Gaussian intensity profile
 *
 * Basic modeling of a Gaussian beam. Injection points are generated on the
 * \f$ z=0 \f$ plane accordingly to given FWHM values specified. Injection
 * directions are set to \f$ \bm{k} = \delta (\bm{k}_z) \f$ regardless of the
 * injection point (see GaussianRayBundleSource for a more elaborate modeling).
 * Time distribution is left unspecified, and must be set serparately.
 */

class GaussianBeamSource : public Source
{
public:
    GaussianBeamSource(double FWHM, BaseObject *parent=NULL);
    GaussianBeamSource(double xFWHM, double yFWHM, BaseObject *parent=NULL);
    Walker *constructWalker() const;
};




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
 * beam waist and at a distance \f$ z = -d, d \gg z_\text{R}\f$. The beauty of
 * this approach is that it not only gets the intensity distribution correct at
 * these two extremes, it also gets it right at all other values of \f$ z \f$
 * (provided that \f$ d \gg z_\text{R}\f$).
 * Time distribution is left unspecified, and must be set serparately.
 */

class GaussianRayBundleSource : public Source
{
public:
    GaussianRayBundleSource(double collimatedWaist, double focusedWaist, double lensDistance, BaseObject *parent=NULL);
    GaussianRayBundleSource(double collimatedXWaist, double collimatedYWaist, double focusedXWaist, double focusedYWaist, double lensDistance, BaseObject *parent=NULL);
    Walker *constructWalker() const;

private:
    double xFoc, yFoc, xColl, yColl;
    double d;
    UniformDistribution *uRand;
};




/**
 * @brief Isotropic point source \f$ \delta (0,0,z_0) \delta (t) \f$.
 */

class IsotropicPointSource : public Source
{
public:
    IsotropicPointSource(double z0, BaseObject *parent=NULL);
    Walker *constructWalker() const;

private:
    double depth;
};

#endif // SOURCE_H
