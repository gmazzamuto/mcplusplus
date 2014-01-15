#ifndef COSTHETAGENERATOR_H
#define COSTHETAGENERATOR_H
#include "distributions.h"

/**
 * \defgroup CosTheta
 *
 * \brief Classes for the generation of \f$ \cos \theta \f$ after a
 * scattering event.
 *
 * \f$ \theta \f$ the polar angle with respect to the
 * incident direction.
 */


/**
 * @brief Isotropic scattering: \f$ \cos \theta \f$ is uniformly generated
 * between -1 and 1.
 * \ingroup CosTheta
 */

class IsotropicCosThetaGenerator : public UniformDistribution
{
public:
    IsotropicCosThetaGenerator(mt19937 *mt);
    IsotropicCosThetaGenerator(int seed);
};



/**
 * @brief Generates a value of \f$ \cos \theta \f$ based on the
 * Henyey-Greenstein phase function, according to given scattering anisotropy
 * factor \f$ g \f$.
 * \ingroup CosTheta
 */

class AnisotropicCosThetaGenerator : public AbstractDistribution
{
public:
    AnisotropicCosThetaGenerator(double g, mt19937 *mt);
    AnisotropicCosThetaGenerator(double g, int seed);
    ~AnisotropicCosThetaGenerator();

    double spin();

private:
    void commonConstructor(double g);

    double g;  /**< @brief scattering anisotropy factor*/
    UniformDistribution *uRandom;
};

#endif // COSTHETAGENERATOR_H
