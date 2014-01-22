#include "distributions.h"


#include <boost/math/constants/constants.hpp>

using namespace boost::math::constants;


// Abstract distribution

AbstractDistribution::AbstractDistribution(BaseObject *parent) :
    BaseObject(parent)
{
}

/**
 * @brief Reconstructs the underlying boost::variate_generator.
 *
 * Derived classes using a distribution which caches random numbers (such as
 * boost::random::normal_distribution) must reimplement this function. This
 * function is called by setSeed() so that numbers generated with the previous
 * seed but not yet used are deleted. In principle this could be simply
 * achieved by calling the reset() method that every boost distribution
 * implements. Anyhow it is useful to have a function like this one, that
 * completely destroys and recreates the variate_generator, to be used in the
 * setter functions for the distribution parameters.
 *
 *
 * The default implementation does nothing.
 */

void AbstractDistribution::reconstructGenerator() {

}

void AbstractDistribution::reset() {
    reconstructGenerator();
}




// Dirac Delta distribution

DeltaDistribution::DeltaDistribution(double center, BaseObject *parent) :
    AbstractDistribution(parent)
{
    x0 = center;
}




// Normal (Gaussian) distribution

NormalDistribution::NormalDistribution(double mean, double sigma, BaseObject *parent) :
    AbstractDistribution(parent)
{
    generator = NULL;
    this->mean = mean;
    this->sigma = sigma;
    reconstructGenerator();
}

NormalDistribution::~NormalDistribution() {
    delete generator;
}

void NormalDistribution::reconstructGenerator() {
    if(generator != NULL)
        delete generator;
    normal_distribution<double> normalDistribution(mean,sigma);
    generator = new normalGenerator(*mt,normalDistribution);
}

void NormalDistribution::setSigma(double value) {
    sigma = value;
    reconstructGenerator();
}

void NormalDistribution::setMean(double value) {
    mean = value;
    reconstructGenerator();
}

/**
 * @brief Sets the FWHM of the normal distribution.
 * @param value
 * \todo Check that this function is working properly.
 */

void NormalDistribution::setFWHM(double value) {
    setSigma(value/(2*root_ln_four<double>()));
}




// Uniform disribution

UniformDistribution::UniformDistribution(double min, double max, BaseObject *parent) :
    AbstractDistribution(parent)
{
    this->min = min;
    this->max = max;
    reconstructGenerator();
}

UniformDistribution::~UniformDistribution() {
    delete generator;
}

void UniformDistribution::reconstructGenerator() {
    if(generator != NULL)
        delete generator;
    uniform_real_distribution<double> uniformRealDistribution(min,max);
    generator = new uniformGenerator(*mt,uniformRealDistribution);
}




// Exponential distribution

ExponentialDistribution::ExponentialDistribution(BaseObject *parent) :
    AbstractDistribution(parent) {
    setLamda(1);
}

ExponentialDistribution::ExponentialDistribution(double lambda, BaseObject *parent) :
    AbstractDistribution(parent)
{
    setLamda(lambda);
}

ExponentialDistribution::~ExponentialDistribution() {
    delete generator;
}

void ExponentialDistribution::reconstructGenerator() {
    if(generator != NULL)
        delete generator;
    exponential_distribution<double> exponentialRealDistribution(lambda);
    generator = new exponentialGenerator(*mt,exponentialRealDistribution);
}

void ExponentialDistribution::setLamda(double value) {
    lambda = value;
    reconstructGenerator();
}




// Sech2 (Logistic) distribution

Sech2Distribution::Sech2Distribution(double mean, double scale, BaseObject *parent) :
    AbstractDistribution(parent)
{
    this->mean = mean;
    this->scale = scale;
    uRandom = new UniformDistribution(0,1,parent);
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

void Sech2Distribution::setScale(double value) {
    scale = value;
}

void Sech2Distribution::setMean(double value) {
    mean = value;
}

/**
 * @brief Sets the FWHM of the sech2 distribution.
 * @param value
 * \todo Check that this function is working properly.
 */

void Sech2Distribution::setFWHM(double value) {
    setScale(value/(4*log(1 + root_two<double>())));
}
