#include "uniformrandomgenerator.h"

UniformRandomGenerator::UniformRandomGenerator(int seed, double min, double max)
{
    mt19937 mersenneTwister = mt19937(seed);
    uniform_real_distribution<double> uniformRealDistribution =
            uniform_real_distribution<double>(min,max);
    generator = new variate_generator<mt19937, uniform_real_distribution<double> >(mersenneTwister,uniformRealDistribution);
}

double UniformRandomGenerator::spin() {
    return generator->operator ()();
}
