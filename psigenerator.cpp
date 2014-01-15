#include "psigenerator.h"

#include <boost/math/constants/constants.hpp>

using namespace boost::math::constants;

PsiGenerator::PsiGenerator(mt19937 *mt) :
    UniformDistribution(0,2*pi<double>(),mt)
{

}

PsiGenerator::PsiGenerator(int seed) :
    UniformDistribution(0,2*pi<double>(),seed)
{

}
