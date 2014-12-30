#include "psigenerator.h"

#include <boost/math/constants/constants.hpp>

using namespace boost::math::constants;
using namespace MCPP;

IsotropicPsiGenerator::IsotropicPsiGenerator(BaseObject *parent) :
    AbstractDistribution(parent)
{

}

MCfloat IsotropicPsiGenerator::spin() const {
    return uniform_01<MCfloat>()(*mt)*two_pi<MCfloat>(); //uniform in [0,2pi)
}

BaseObject *IsotropicPsiGenerator::clone_impl() const
{
    return new IsotropicPsiGenerator();
}
