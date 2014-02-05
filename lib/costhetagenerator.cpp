#include "costhetagenerator.h"

CosThetaGenerator::CosThetaGenerator(double g, BaseObject *parent) :
    AbstractDistribution(parent)
{
    setg(g);
}

void CosThetaGenerator::setg(double g) {
    this->g = g;
}

double CosThetaGenerator::spin() const {
    if(g==0.) {
        return uniform_01<double>()(*mt)*2.-1.; //uniform in [-1,1)
    }
    else {
        double temp;
        temp = 1 - g + 2*g*uniform_01<double>()(*mt);
        temp = (1-g*g)/temp;
        temp = 1 + g*g - temp*temp;
        temp = temp/(2*g);
        return temp;
    }
}
