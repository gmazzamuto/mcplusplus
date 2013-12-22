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
    inline double spin();

private:
    double x0;
};


typedef variate_generator<mt19937, uniform_real_distribution<double> > uniformGenerator;

class UniformRandomDistribution : public AbstractDistribution
{
public:
    UniformRandomDistribution(int seed, double min = 0, double max = 1);
    ~UniformRandomDistribution();

    double spin();

private:
    uniformGenerator *generator;
};




typedef variate_generator<mt19937, exponential_distribution<double> > exponentialGenerator;


class ExponentialRandomDistribution : public AbstractDistribution
{
public:
    ExponentialRandomDistribution(int seed, double lambda);
    ~ExponentialRandomDistribution();

    double spin();

private:
    exponentialGenerator *generator;
};


#endif // DISTRIBUTIONS_H
