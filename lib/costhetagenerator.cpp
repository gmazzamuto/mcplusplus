#include "costhetagenerator.h"

using namespace MCPP;

CosThetaGenerator::CosThetaGenerator(MCfloat g, BaseObject *parent) :
    AbstractDistribution(parent)
{
    setg(g);
}

void CosThetaGenerator::setg(MCfloat g) {
    this->g = g;
}

MCfloat CosThetaGenerator::spin() const {
    if(g==0.) {
        return uniform_01<MCfloat>()(*mt)*2.-1.; //uniform in [-1,1)
    }
    else {
        MCfloat temp;
        const MCfloat xi = uniform_01<MCfloat>()(*mt);
        temp = 1 - g + 2*g*xi;
        temp = (1-g*g)/temp;
        temp = 1 + g*g - temp*temp;
        temp = temp/(2*g);
        if(temp < -1.)
            temp = -1.;
        else if(temp > 1.)
            temp = 1.;
        return temp;
    }
}

BaseObject *CosThetaGenerator::clone_impl() const
{
    return new CosThetaGenerator(g);
}
