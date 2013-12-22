#include "distributions.h"


AbstractDistribution::AbstractDistribution() {

}

AbstractDistribution::~AbstractDistribution() {

}



DeltaDistribution::DeltaDistribution(double center) :
    AbstractDistribution()
{
    x0 = center;
}

double DeltaDistribution::spin() {
    return x0;
}


UniformRandomDistribution::UniformRandomDistribution(int seed, double min, double max) :
    AbstractDistribution()
{
    mt19937 mersenneTwister(seed);
    uniform_real_distribution<double> uniformRealDistribution(min,max);
    generator = new uniformGenerator(mersenneTwister,uniformRealDistribution);
}

UniformRandomDistribution::~UniformRandomDistribution() {
    delete generator;
}

double UniformRandomDistribution::spin() {
    return (*generator)();
}




ExponentialRandomDistribution::ExponentialRandomDistribution(int seed, double lambda) :
    AbstractDistribution()
{
   mt19937 mersenneTwister(seed);
   exponential_distribution<double> exponentialRealDistribution(lambda);
   generator = new exponentialGenerator(mersenneTwister,exponentialRealDistribution);
}

ExponentialRandomDistribution::~ExponentialRandomDistribution() {
    delete generator;
}

double ExponentialRandomDistribution::spin() {
    return (*generator)();
}
