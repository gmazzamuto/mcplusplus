#include "uniformrandomgenerator.h"

UniformRandomGenerator::UniformRandomGenerator(int seed)
{
    boost::random::mt19937 mersenneTwister = boost::random::mt19937(seed);
    boost::random::uniform_real_distribution<double> uniformRealDistribution =
            boost::random::uniform_real_distribution<double>(0,1);
    generator = new boost::random::variate_generator<boost::random::mt19937, boost::random::uniform_real_distribution<double> >(mersenneTwister,uniformRealDistribution);
}

double UniformRandomGenerator::generate() {
    return generator->operator ()();
}
