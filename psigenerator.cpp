#include "psigenerator.h"

#include <boost/math/constants/constants.hpp>

using namespace boost::math::constants;

PsiGenerator::PsiGenerator(BaseObject *parent) :
    UniformDistribution(0,2*pi<double>(),parent)
{

}
