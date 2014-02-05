#ifndef COSTHETAGENERATOR_H
#define COSTHETAGENERATOR_H
#include "distributions.h"

/**
 * @brief Spins the value of \f$ \cos \theta \f$ after a scattering event in
 * accordance with given anisotropy factor \f$ g \f$
 */

class CosThetaGenerator : public AbstractDistribution
{
public:
    CosThetaGenerator(double g, BaseObject *parent=NULL);

    void setg(double g);
    double spin() const;

private:
    double g;  /**< @brief scattering anisotropy factor*/
};

#endif // COSTHETAGENERATOR_H
