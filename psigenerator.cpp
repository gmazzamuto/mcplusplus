#include "psigenerator.h"

#include <boost/math/constants/constants.hpp>

using namespace boost::math::constants;

IsotropicPsiGenerator::IsotropicPsiGenerator(BaseObject *parent) :
    UniformDistribution(0,2*pi<double>(),parent)
{

}
