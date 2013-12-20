#ifndef UNIFORMRANDOMGENERATOR_H
#define UNIFORMRANDOMGENERATOR_H

#include <boost/random/variate_generator.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/discrete_distribution.hpp>
#include <boost/random/uniform_real_distribution.hpp>

class UniformRandomGenerator
{
public:
    UniformRandomGenerator(int seed);
    double generate();

private:
    boost::random::variate_generator<boost::random::mt19937, boost::random::uniform_real_distribution<double> > *generator;
};


#endif // UNIFORMRANDOMGENERATOR_H
