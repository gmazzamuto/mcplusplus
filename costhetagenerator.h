#ifndef COSTHETAGENERATOR_H
#define COSTHETAGENERATOR_H
#include "distributions.h"

class AbstractCosThetaGenerator
{
public:
    AbstractCosThetaGenerator(mt19937 *mt = NULL, double min = 0, double max = 1);
    virtual ~AbstractCosThetaGenerator();

    virtual double spin() = 0;


protected:
    UniformDistribution *uRandom;
};




class IsotropicCosThetaGenerator : public AbstractCosThetaGenerator
{
public:
    IsotropicCosThetaGenerator(mt19937 *mt);
    double spin();
};




class AnisotropicCosThetaGenerator : public AbstractCosThetaGenerator
{
public:
    AnisotropicCosThetaGenerator(mt19937 *mt, double g);
    double spin();

private:
    double g;
};

#endif // COSTHETAGENERATOR_H
