#ifndef DISTRIBUTIONS_H
#define DISTRIBUTIONS_H

#include <boost/random.hpp>

using namespace boost;
using namespace boost::random;


class AbstractDistribution
{
public:
    AbstractDistribution();
    virtual ~AbstractDistribution();

    virtual double spin() = 0;
};



class DeltaDistribution : public AbstractDistribution
{
public:
    DeltaDistribution(double center);
    inline double spin() {return x0;}

private:
    double x0;
};



typedef variate_generator<mt19937, normal_distribution<double>  > normalGenerator;

class NormalDistribution : public AbstractDistribution
{
public:
    NormalDistribution(int seed, double mean, double sigma);
    ~NormalDistribution();

    inline double spin() {return (*generator)();}

private:
    normalGenerator *generator;
};




typedef variate_generator<mt19937, uniform_real_distribution<double> > uniformGenerator;

class UniformDistribution : public AbstractDistribution
{
public:
    UniformDistribution(int seed, double min = 0, double max = 1);
    ~UniformDistribution();

    inline double spin() {return (*generator)();}

private:
    uniformGenerator *generator;
};




typedef variate_generator<mt19937, exponential_distribution<double> > exponentialGenerator;


class ExponentialDistribution : public AbstractDistribution
{
public:
    ExponentialDistribution(int seed, double lambda);
    ~ExponentialDistribution();

    inline double spin() {return (*generator)();}

private:
    exponentialGenerator *generator;
};


#endif // DISTRIBUTIONS_H
