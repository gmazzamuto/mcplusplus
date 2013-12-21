#ifndef COSTHETAGENERATOR_H
#define COSTHETAGENERATOR_H
#include "uniformrandomgenerator.h"

class AbstractCosThetaGenerator
{
public:
    AbstractCosThetaGenerator(int seed, double min = 0, double max = 1);
    virtual double spin() = 0;

protected:
    UniformRandomGenerator *uRandom;
};

class IsotropicCosThetaGenerator : public AbstractCosThetaGenerator
{
public:
    IsotropicCosThetaGenerator(int seed);
    double spin();
};

class AnisotropicCosThetaGenerator : public AbstractCosThetaGenerator
{
public:
    AnisotropicCosThetaGenerator(int seed, double g);
    double spin();

private:
    double g;
};

#endif // COSTHETAGENERATOR_H
