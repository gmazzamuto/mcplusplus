#include "distributions.h"



/**
 * @brief Base distribution class. Provides a common interface to handle various distributions.
 */

AbstractDistribution::AbstractDistribution() {}

AbstractDistribution::~AbstractDistribution() {}



/**
 * @brief Dirac Delta distribution \f$ f(x) = \delta (x) \f$
 * @param center
 */

DeltaDistribution::DeltaDistribution(double center) :
    AbstractDistribution()
{
    x0 = center;
}



/**
 * @brief Normal (Gaussian) distribution \f$ f(x) = (2\pi \sigma)^{-\frac{1}{2}} \exp \left( -\frac{(x-\mu)^2}{2 \sigma^2} \right) \f$
 * @param seed
 * @param mean
 * @param sigma
 */

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



/**
 * @brief Uniform real distribution
 * @param seed
 * @param min
 * @param max
 */

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



/**
 * @brief Exponential distribution \f$ f(x) = \lambda \exp ( -\lambda x) \f$
 * @param seed
 * @param lambda
 */

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



/**
 * @brief Hyperbolic secant square distribution \f$ f(x) = (4s)^{-1} \sech^2 \left( \frac{x - \mu}{2s} \right) \f$
 * @param seed
 * @param mean
 * @param scale
 */

Sech2Distribution::Sech2Distribution(int seed, double mean, double scale) :
    AbstractDistribution()
{
    this->mean = mean;
    this->scale = scale;
    uRandom = new UniformDistribution(seed,0,1);
}

Sech2Distribution::~Sech2Distribution() {
    delete uRandom;
}

double Sech2Distribution::spin() {
    double temp;
    double tempRand;
    tempRand = uRandom->spin();
    temp = 1. - tempRand;
    temp = tempRand/temp;
    temp = log(temp);
    temp = mean + scale*temp;
    return temp;
}
