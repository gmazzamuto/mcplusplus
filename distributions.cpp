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




NormalDistribution::NormalDistribution(int seed, double mean, double sigma) :
    AbstractDistribution()
{
    mt19937 mersenneTwister(seed);
    normal_distribution<double> normalDistribution(mean,sigma);
    generator = new normalGenerator(mersenneTwister,normalDistribution);
}

NormalDistribution::~NormalDistribution() {
    delete generator;
}



UniformDistribution::UniformDistribution(int seed, double min, double max) :
    AbstractDistribution()
{
    mt19937 mersenneTwister(seed);
    uniform_real_distribution<double> uniformRealDistribution(min,max);
    generator = new uniformGenerator(mersenneTwister,uniformRealDistribution);
}

UniformDistribution::~UniformDistribution() {
    delete generator;
}




ExponentialDistribution::ExponentialDistribution(int seed, double lambda) :
    AbstractDistribution()
{
   mt19937 mersenneTwister(seed);
   exponential_distribution<double> exponentialRealDistribution(lambda);
   generator = new exponentialGenerator(mersenneTwister,exponentialRealDistribution);
}

ExponentialDistribution::~ExponentialDistribution() {
    delete generator;
}
