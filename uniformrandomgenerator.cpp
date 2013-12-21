#include "uniformrandomgenerator.h"

UniformRandomGenerator::UniformRandomGenerator(int seed, double min, double max)
{
    boost::random::mt19937 mersenneTwister = boost::random::mt19937(seed);
    boost::random::uniform_real_distribution<double> uniformRealDistribution =
            boost::random::uniform_real_distribution<double>(min,max);
    generator = new boost::random::variate_generator<boost::random::mt19937, boost::random::uniform_real_distribution<double> >(mersenneTwister,uniformRealDistribution);
}

double UniformRandomGenerator::spin() {
    return generator->operator ()();
}
