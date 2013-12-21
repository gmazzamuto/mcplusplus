#ifndef UNIFORMRANDOMGENERATOR_H
#define UNIFORMRANDOMGENERATOR_H

#include <boost/random/variate_generator.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/uniform_real_distribution.hpp>

using namespace boost::random;

class UniformRandomGenerator
{
public:
    UniformRandomGenerator(int seed, double min = 0, double max = 1);
    double spin();

private:
    variate_generator<mt19937, uniform_real_distribution<double> > *generator;
};


#endif // UNIFORMRANDOMGENERATOR_H
