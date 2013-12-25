#ifndef PSIGENERATOR_H
#define PSIGENERATOR_H
#include "distributions.h"

/**
 * @brief The PsiGenerator class generates the azimuthal scattering angle uniformly in the interval \f$ \psi \in [0, 2\pi) \f$.
 */

class PsiGenerator
{
public:
    PsiGenerator(mt19937 *mt = NULL);
    virtual ~PsiGenerator();

    double spin();

private:
    UniformDistribution *uRandom;

};

#endif // PSIGENERATOR_H
