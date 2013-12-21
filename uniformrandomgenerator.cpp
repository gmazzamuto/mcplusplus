#include "uniformrandomgenerator.h"

UniformRandomGenerator::UniformRandomGenerator(int seed, double min, double max)
{
    mt19937 mersenneTwister(seed);
    uniform_real_distribution<double> uniformRealDistribution(min,max);
    generator = new uniformGenerator(mersenneTwister,uniformRealDistribution);
}

double UniformRandomGenerator::spin() {
    return (*generator)();
}

ExponentialRandomGenerator::ExponentialRandomGenerator(int seed, double lambda)
{
   mt19937 mersenneTwister(seed);
   exponential_distribution<double> exponentialRealDistribution(lambda);
   generator = new exponentialGenerator(mersenneTwister,exponentialRealDistribution);
}

double ExponentialRandomGenerator::spin() {
    return (*generator)();
}
