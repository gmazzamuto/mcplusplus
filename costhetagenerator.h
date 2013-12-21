#ifndef COSTHETAGENERATOR_H
#define COSTHETAGENERATOR_H

class AbstractCosThetaGenerator
{
public:
    AbstractCosThetaGenerator(int seed);
    virtual double spin() = 0;

private:
    int seed;
};

#endif // COSTHETAGENERATOR_H
