#include "costhetagenerator.h"

IsotropicCosThetaGenerator::IsotropicCosThetaGenerator(BaseObject *parent) :
    UniformDistribution(-1,1,parent)
{

}




AnisotropicCosThetaGenerator::AnisotropicCosThetaGenerator(double g, BaseObject *parent) :
    AbstractDistribution(parent)
{
    setg(g);
}

void AnisotropicCosThetaGenerator::setg(double g) {
    this->g = g;
    uRandom = new UniformDistribution(0,1,this);
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
