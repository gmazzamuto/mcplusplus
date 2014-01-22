#ifndef PSIGENERATOR_H
#define PSIGENERATOR_H
#include "distributions.h"

/**
 * @brief The PsiGenerator class generates the azimuthal scattering angle
 * uniformly in the interval \f$ \psi \in [0, 2\pi) \f$.
 */

class PsiGenerator : public UniformDistribution
{
public:
    PsiGenerator(BaseObject *parent=NULL);
};

#endif // PSIGENERATOR_H
