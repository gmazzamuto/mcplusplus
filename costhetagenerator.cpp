#include "costhetagenerator.h"

IsotropicCosThetaGenerator::IsotropicCosThetaGenerator(BaseObject *parent) :
    UniformDistribution(-1,1,parent)
{

}




AnisotropicCosThetaGenerator::AnisotropicCosThetaGenerator(double g, BaseObject *parent) :
    UniformDistribution(0,1,parent)
{
    setg(g);
}

void AnisotropicCosThetaGenerator::setg(double g) {
    this->g = g;
}

double AnisotropicCosThetaGenerator::spin() {
    double temp;
    temp = 1 - g + 2*g*UniformDistribution::spin();
    temp = (1-g*g)/temp;
    temp = 1 + g*g - temp*temp;
    temp = temp/(2*g);
    return temp;
}
