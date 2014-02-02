#include "psigenerator.h"

#include <boost/math/constants/constants.hpp>

using namespace boost::math::constants;

IsotropicPsiGenerator::IsotropicPsiGenerator(BaseObject *parent) :
    AbstractDistribution(parent)
{

}

double IsotropicPsiGenerator::spin() const {
    return uniform_01<double>()(*mt)*two_pi<double>(); //uniform in [0,2pi)
}
