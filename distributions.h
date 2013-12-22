#ifndef DISTRIBUTIONS_H
#define DISTRIBUTIONS_H

#include <boost/random.hpp>
#include <math.h>
//#include <boost/math/distributions.hpp>

using namespace boost;
using namespace boost::random;
using namespace boost::math;

// Abstract distribution

class AbstractDistribution
{
public:
    AbstractDistribution();
    virtual ~AbstractDistribution();

    virtual double spin() = 0;
};


// Dirac Delta distribution

class DeltaDistribution : public AbstractDistribution
{
public:
    DeltaDistribution(double center);
    inline double spin() {return x0;}

private:
    double x0;
};


// Normal (Gaussian) distribution

typedef variate_generator<mt19937, normal_distribution<double> > normalGenerator;

class NormalDistribution : public AbstractDistribution
{
public:
    NormalDistribution(int seed, double mean, double sigma);
    ~NormalDistribution();

    inline double spin() {return (*generator)();}

private:
    normalGenerator *generator;
};


// Uniform disribution

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


// Exponential distribution

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

// Sech2 (Logistic) distribution - see <a href="http://en.wikipedia.org/wiki/Logistic_distribution">Wikpiedia</a> for further reference on quantile function

class Sech2Distribution : public AbstractDistribution
{
public:
    Sech2Distribution(int seed, double mean, double scale);
    ~Sech2Distribution();

    double spin();

private:
    UniformDistribution *uRandom;
    double mean;
    double scale;
};


#endif // DISTRIBUTIONS_H
