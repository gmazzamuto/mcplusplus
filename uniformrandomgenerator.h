#ifndef UNIFORMRANDOMGENERATOR_H
#define UNIFORMRANDOMGENERATOR_H

#include <boost/random.hpp>

using namespace boost;
using namespace boost::random;

typedef variate_generator<mt19937, uniform_real_distribution<double> > uniformGenerator;

class UniformRandomGenerator
{
public:
    UniformRandomGenerator(int seed, double min = 0, double max = 1);
    double spin();

private:
    uniformGenerator *generator;
};

typedef variate_generator<mt19937, exponential_distribution<double> > exponentialGenerator;


class ExponentialRandomGenerator
{
public:
    ExponentialRandomGenerator(int seed, double lambda);
    double spin();

private:
    exponentialGenerator *generator;
};


#endif // UNIFORMRANDOMGENERATOR_H
