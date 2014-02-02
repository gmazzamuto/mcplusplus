#include "costhetagenerator.h"

IsotropicCosThetaGenerator::IsotropicCosThetaGenerator(BaseObject *parent) :
    AbstractDistribution(parent)
{

}

double IsotropicCosThetaGenerator::spin() const {
    return uniform_01<double>()(*mt)*2.-1.; //uniform in [-1,1)
}


AnisotropicCosThetaGenerator::AnisotropicCosThetaGenerator(double g, BaseObject *parent) :
    AbstractDistribution(parent)
{
    setg(g);
}

void AnisotropicCosThetaGenerator::setg(double g) {
    this->g = g;
}

double AnisotropicCosThetaGenerator::spin() const {
    double temp;
    temp = 1 - g + 2*g*uniform_01<double>()(*mt);
    temp = (1-g*g)/temp;
    temp = 1 + g*g - temp*temp;
    temp = temp/(2*g);
    return temp;
}
