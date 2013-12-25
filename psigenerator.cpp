#include "psigenerator.h"

#include <boost/math/constants/constants.hpp>

using namespace boost::math::constants;

PsiGenerator::PsiGenerator(mt19937 *mt)
{
    uRandom = new UniformDistribution(mt,0,2*pi<double>());
}

PsiGenerator::~PsiGenerator()
{
    delete uRandom;
}

double PsiGenerator::spin() {
    return uRandom->spin();
}
