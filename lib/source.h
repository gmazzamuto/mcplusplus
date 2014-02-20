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
    void setWavelength(double um);
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

private:
    virtual void spinDirection(Walker *walker) const;
    virtual void spinPosition(Walker *walker) const;
    virtual void spinTime(Walker *walker) const;
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

private:
    virtual void spinDirection(Walker *walker) const;
};





/**
 * @brief Isotropic point source \f$ \delta (0,0,z_0) \delta (t) \f$.
 */

class IsotropicPointSource : public Source
{
public:
    IsotropicPointSource(double z0, BaseObject *parent=NULL);

private:
    virtual void spinPosition(Walker *walker) const;
    virtual void spinTime(Walker *walker) const;

    double depth;
};

#endif // SOURCE_H
