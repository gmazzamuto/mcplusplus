#include "costhetagenerator.h"

IsotropicCosThetaGenerator::IsotropicCosThetaGenerator(mt19937 *mt) :
    UniformDistribution(-1,1,mt)
{

}

IsotropicCosThetaGenerator::IsotropicCosThetaGenerator(int seed) :
    UniformDistribution(-1,1,seed)
{

}




AnisotropicCosThetaGenerator::AnisotropicCosThetaGenerator(double g, mt19937 *mt) :
    AbstractDistribution(mt)
{
    commonConstructor(g);
}

AnisotropicCosThetaGenerator::AnisotropicCosThetaGenerator(double g, int seed) :
    AbstractDistribution(seed)
{
    commonConstructor(g);
}

void AnisotropicCosThetaGenerator::commonConstructor(double g) {
    this->g = g;
    uRandom = new UniformDistribution(0,1,mt);
}

AnisotropicCosThetaGenerator::~AnisotropicCosThetaGenerator() {
    delete uRandom;
}

double AnisotropicCosThetaGenerator::spin() {
    double temp;
    temp = 1 - g + 2*g*uRandom->spin();
    temp = (1-g*g)/temp;
    temp = 1 + g*g - temp*temp;
    temp = temp/(2*g);
    return temp;
}
