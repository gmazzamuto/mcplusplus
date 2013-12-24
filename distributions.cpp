#include "distributions.h"



/**
 * @brief Base distribution class. Provides a common interface to handle various distributions.
 */

AbstractDistribution::AbstractDistribution(mt19937 *mt) {
    this->mt = mt;
}

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

NormalDistribution::NormalDistribution(mt19937 *mt, double mean, double sigma) :
    AbstractDistribution(mt)
{
    normal_distribution<double> normalDistribution(mean,sigma);
    generator = new normalGenerator(*mt,normalDistribution);
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

UniformDistribution::UniformDistribution(mt19937 *mt, double min, double max) :
    AbstractDistribution(mt)
{
    uniform_real_distribution<double> uniformRealDistribution(min,max);
    generator = new uniformGenerator(*mt,uniformRealDistribution);
}

UniformDistribution::~UniformDistribution() {
    delete generator;
}



/**
 * @brief Exponential distribution \f$ f(x) = \lambda \exp ( -\lambda x) \f$
 * @param seed
 * @param lambda
 */

ExponentialDistribution::ExponentialDistribution(mt19937 *mt, double lambda) :
    AbstractDistribution(mt)
{
   exponential_distribution<double> exponentialRealDistribution(lambda);
   generator = new exponentialGenerator(*mt,exponentialRealDistribution);
}

ExponentialDistribution::~ExponentialDistribution() {
    delete generator;
}



/**
 * @brief Hyperbolic secant square distribution \f$ f(x) = (4s)^{-1} \, \textup{sech}^2 \left( \frac{x - \mu}{2s} \right) \f$
 * @param seed
 * @param mean
 * @param scale
 */

Sech2Distribution::Sech2Distribution(mt19937 *mt, double mean, double scale) :
    AbstractDistribution()
{
    this->mean = mean;
    this->scale = scale;
    uRandom = new UniformDistribution(mt,0,1);
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
