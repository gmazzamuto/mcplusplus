#ifndef COSTHETAGENERATOR_H
#define COSTHETAGENERATOR_H
#include "distributions.h"

/**
 * @brief Base class to generate the value of \f$ \cos \theta \f$ after a scattering event, being \f$ \theta \f$ the polar angle with respect to the incident direction.
 */

class AbstractCosThetaGenerator
{
public:
    AbstractCosThetaGenerator(mt19937 *mt = NULL, double min = 0, double max = 1);
    virtual ~AbstractCosThetaGenerator();

    virtual double spin() = 0;


protected:
    UniformDistribution *uRandom;
};



/**
 * @brief Isotropic scattering: \f$ \cos \theta \f$ is uniformly generated between -1 and 1.
 */

class IsotropicCosThetaGenerator : public AbstractCosThetaGenerator
{
public:
    IsotropicCosThetaGenerator(mt19937 *mt);
    double spin();
};



/**
 * @brief Generates a value of \f$ \cos \theta \f$ based on the Henyey-Greenstein phase function, according to given scattering anisotropy factor \f$ g \f$.
 */

class AnisotropicCosThetaGenerator : public AbstractCosThetaGenerator
{
public:
    AnisotropicCosThetaGenerator(mt19937 *mt, double g);
    double spin();

private:
    double g;  /**< @brief scattering anisotropy factor*/
};

#endif // COSTHETAGENERATOR_H
