#include "costhetagenerator.h"

AbstractCosThetaGenerator::AbstractCosThetaGenerator(int seed, double min, double max)
{
    uRandom = new UniformDistribution(seed,min,max);
}

AbstractCosThetaGenerator::~AbstractCosThetaGenerator()
{
    delete uRandom;
}



IsotropicCosThetaGenerator::IsotropicCosThetaGenerator(int seed) :
    AbstractCosThetaGenerator(seed,-1,1)
{

}

double IsotropicCosThetaGenerator::spin() {
    return uRandom->spin();
}



AnisotropicCosThetaGenerator::AnisotropicCosThetaGenerator(int seed, double g) :
    AbstractCosThetaGenerator(seed,0,1)
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
