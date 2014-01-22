#ifndef SOURCE_H
#define SOURCE_H

#include "walker.h"
#include "distributions.h"

/**
 * @brief Base Source class: constructs and initializes a Walker according to
 * given source term modeling.
 */

class Source : public BaseObject
{
public:
    Source(BaseObject *parent=NULL);
    virtual Walker *constructWalker();
    void setr0Distribution(AbstractDistribution **distrArray);
    void setk0Distribution(AbstractDistribution **distrArray);
    void setWalkTimeDistribution(AbstractDistribution *distr);
    void setWavelength(double nm);
    double wavelength();

protected:
    AbstractDistribution *r0Distribution[3];
    AbstractDistribution *k0Distribution[3];
    AbstractDistribution *walkTimeDistribution;

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
    Walker *constructWalker();
};


/**
 * @brief 2-dimensional \f$ xy \f$ Gaussian intensity profile (\f$z = 0\f$).
 *
 * Time distribution is left unspecified, \f$ \bm{k} = (0,0,1) \f$.
 * Time distribution must be set separately, and is advisable to override the
 * default \f$ \delta(\bm{k}_z) \f$ distribution as well if the Rayleigh length
 * of the beam \f$ \sim w_0^2 / \lambda \f$ is much shorter than the
 * scattering/transport length of the entrance layer.
 */

class GaussianBeamSource : public Source
{
public:
    GaussianBeamSource(double FWHM, BaseObject *parent=NULL);
    GaussianBeamSource(double xFWHM, double yFWHM, BaseObject *parent=NULL);
    Walker *constructWalker();
};

#endif // SOURCE_H
