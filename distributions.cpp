#include "distributions.h"

#include <boost/math/constants/constants.hpp>

using namespace boost::math::constants;




// Abstract distribution

AbstractDistribution::AbstractDistribution(BaseObject *parent) :
    BaseRandom(parent)
{
}

/**
 * @brief Reconstructs the underlying distribution.
 *
 * This function is called by setRNG() and reset(). This is useful for
 * distributions that cache random numbers (such as
 * boost::random::normal_distribution) so that numbers generated with the
 * previous seed but not yet used are deleted. In principle this could be simply
 * achieved by calling the %reset() method that every boost distribution
 * implements. Anyhow it is useful to have a function like this one, that
 * recreates the distribution, to be used in the setter functions for the
 * distribution parameters.
 *
 * The default implementation does nothing.
 */

void AbstractDistribution::reconstructDistribution() {

}

/**
 * @brief Resets the distribution.
 *
 * Subsequent calls to spin() do not depend on values produced by the RNG engine
 * prior to invoking reset.
 *
 * Calls reconstructDistribution().
 */

void AbstractDistribution::reset() {
    reconstructDistribution();
}

void AbstractDistribution::setRNG_impl() {
    reset();
}




// Dirac Delta distribution

DeltaDistribution::DeltaDistribution(double center, BaseObject *parent) :
    AbstractDistribution(parent)
{
    setCenter(center);
}

void DeltaDistribution::setCenter(double val) {
    x0 = val;
}

double DeltaDistribution::spin() const {
    return x0;
}




// Normal (Gaussian) distribution

NormalDistribution::NormalDistribution(double mean, double sigma, BaseObject *parent) :
    AbstractDistribution(parent)
{
    this->mean = mean;
    this->sigma = sigma;
    reconstructDistribution();
}

NormalDistribution::~NormalDistribution() {
    delete distribution;
}

void NormalDistribution::reconstructDistribution() {
    distribution = new normal_distribution<double>(mean,sigma);
}

void NormalDistribution::setSigma(double value) {
    sigma = value;
    reconstructDistribution();
}

void NormalDistribution::setMean(double value) {
    mean = value;
    reconstructDistribution();
}

/**
 * @brief Sets the FWHM of the normal distribution.
 * @param value
 * \todo Check that this function is working properly.
 */

void NormalDistribution::setFWHM(double value) {
    setSigma(value/(2*root_ln_four<double>()));
}

double NormalDistribution::spin() const {
    return (*distribution)(*mt);
}




// Uniform disribution

UniformDistribution::UniformDistribution(double min, double max, BaseObject *parent) :
    AbstractDistribution(parent)
{
    this->min = min;
    this->max = max;
    reconstructDistribution();
}

void UniformDistribution::reconstructDistribution() {
    distribution = uniform_real_distribution<double>(min,max);
}

double UniformDistribution::spin() const {
    return distribution(*mt);
}




// Exponential distribution

/**
 * @brief Constructs an exponential distribution with \f$ \lambda=1 \f$
 * @param parent
 */

ExponentialDistribution::ExponentialDistribution(BaseObject *parent) :
    AbstractDistribution(parent)
{
    setLambda(1);
}

/**
 * @brief Constructs an exponential distribution with the given \f$ \lambda \f$
 * @param lambda
 * @param parent
 */

ExponentialDistribution::ExponentialDistribution(double lambda, BaseObject *parent) :
    AbstractDistribution(parent)
{
    setLambda(lambda);
}

void ExponentialDistribution::reconstructDistribution() {
    distribution = exponential_distribution<double>(lambda);
}

/**
 * @brief Convenience function to set \f$ \beta = \frac{1}{\lambda} \f$
 * @param value
 */

void ExponentialDistribution::setBeta(double value) {
    setLambda(1./value);
}

/**
 * @brief Sets the rate \f$ \lambda \f$
 * @param value
 */

void ExponentialDistribution::setLambda(double value) {
    lambda = value;
    reconstructDistribution();
}

double ExponentialDistribution::spin() const {
    return distribution(*mt);
}




// Sech2 (Logistic) distribution

Sech2Distribution::Sech2Distribution(double mean, double scale, BaseObject *parent) :
    AbstractDistribution(parent)
{
    this->mean = mean;
    this->scale = scale;
}

double Sech2Distribution::spin() const {
    double p;
    do {
        p = uniform_01<double>()(*mt);
    }
    while(p == 0);
    return  mean + scale*log(p/(1.-p));
}

void Sech2Distribution::setScale(double value) {
    scale = value;
}

void Sech2Distribution::setMean(double value) {
    mean = value;
}

/**
 * @brief Sets the FWHM of the \f$ \textup{sech}^2 \f$ distribution.
 * @param value
 *
 *
 * Convenience function to set the scale
 * \f$ s = \dfrac{\textup{FWHM}}{4\ln(1+\sqrt{2})} \f$
 * \todo Check that this function is working properly.
 */

void Sech2Distribution::setFWHM(double value) {
    setScale(value/(4*log(1 + root_two<double>())));
}
