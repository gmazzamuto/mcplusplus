#ifndef COSTHETAGENERATOR_H
#define COSTHETAGENERATOR_H
#include "distributions.h"

namespace MCPP {

/**
 * @brief Spins the value of \f$ \cos \theta \f$ after a scattering event in
 * accordance with given anisotropy factor \f$ g \f$
 */

class CosThetaGenerator : public AbstractDistribution
{
public:
    CosThetaGenerator(double g=0, BaseObject *parent=NULL);

    void setg(double g);
    MCfloat spin() const;

private:
    virtual BaseObject *clone_impl() const;
    MCfloat g;  /**< @brief scattering anisotropy factor*/
};

}
#endif // COSTHETAGENERATOR_H
