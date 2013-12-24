#include "costhetagenerator.h"

AbstractCosThetaGenerator::AbstractCosThetaGenerator(mt19937 *mt, double min, double max)
{
    uRandom = new UniformDistribution(mt,min,max);
}

AbstractCosThetaGenerator::~AbstractCosThetaGenerator()
{
    delete uRandom;
}



IsotropicCosThetaGenerator::IsotropicCosThetaGenerator(mt19937 *mt) :
    AbstractCosThetaGenerator(mt,-1,1)
{

}

double IsotropicCosThetaGenerator::spin() {
    return uRandom->spin();
}



AnisotropicCosThetaGenerator::AnisotropicCosThetaGenerator(mt19937 *mt, double g) :
    AbstractCosThetaGenerator(mt,0,1)
{
    this->g = g;
}

double AnisotropicCosThetaGenerator::spin() {
    double temp;
    temp = 1 - g + 2*g*uRandom->spin();
    temp = (1-g*g)/temp;
    temp = 1 + g*g - temp*temp;
    temp = temp/(2*g);
    return temp;
}
