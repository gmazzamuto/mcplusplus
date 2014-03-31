#include "psigenerator.h"

#include <boost/math/constants/constants.hpp>

using namespace boost::math::constants;

IsotropicPsiGenerator::IsotropicPsiGenerator(BaseObject *parent) :
    AbstractDistribution(parent)
{

}

MCfloat IsotropicPsiGenerator::spin() const {
    return uniform_01<MCfloat>()(*mt)*two_pi<MCfloat>(); //uniform in [0,2pi)
}
