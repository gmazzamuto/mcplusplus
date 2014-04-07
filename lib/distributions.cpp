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

DeltaDistribution::DeltaDistribution(MCfloat center, BaseObject *parent) :
    AbstractDistribution(parent)
{
    setCenter(center);
}

void DeltaDistribution::setCenter(MCfloat val) {
    x0 = val;
}

MCfloat DeltaDistribution::spin() const {
    return x0;
}

BaseObject * DeltaDistribution::clone_impl() const
{
    return new DeltaDistribution(x0);
}




// Normal (Gaussian) distribution

NormalDistribution::NormalDistribution(MCfloat mean, MCfloat sigma, BaseObject *parent) :
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
    distribution = new normal_distribution<MCfloat>(mean,sigma);
}

BaseObject *NormalDistribution::clone_impl() const
{
    return new NormalDistribution(mean,sigma);
}

void NormalDistribution::setSigma(MCfloat value) {
    sigma = value;
    reconstructDistribution();
}

void NormalDistribution::setMean(MCfloat value) {
    mean = value;
    reconstructDistribution();
}

/**
 * @brief Sets the FWHM of the normal distribution.
 * @param value
 * \todo Check that this function is working properly.
 */

void NormalDistribution::setFWHM(MCfloat value) {
    setSigma(value/(2*root_ln_four<MCfloat>()));
}

MCfloat NormalDistribution::spin() const {
    return (*distribution)(*mt);
}




// Uniform disribution

UniformDistribution::UniformDistribution(MCfloat min, MCfloat max, BaseObject *parent) :
    AbstractDistribution(parent)
{
    this->min = min;
    this->max = max;
    reconstructDistribution();
}

void UniformDistribution::reconstructDistribution() {
    distribution = uniform_real_distribution<MCfloat>(min,max);
}

BaseObject *UniformDistribution::clone_impl() const
{
    return new UniformDistribution(min,max);
}

MCfloat UniformDistribution::spin() const {
    return distribution(*mt);
}

/**
 * @brief Spins a number uniformly distributed in the open interval
 *        \f$ [\textup{min,max}) \f$
 * @return
 */

MCfloat UniformDistribution::spinOpen() const {
    MCfloat p;
    do {
        p = spin();
    }
    while (p == min);
    return p;
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

ExponentialDistribution::ExponentialDistribution(MCfloat lambda, BaseObject *parent) :
    AbstractDistribution(parent)
{
    setLambda(lambda);
}

void ExponentialDistribution::reconstructDistribution() {
    distribution = exponential_distribution<MCfloat>(lambda);
}

BaseObject *ExponentialDistribution::clone_impl() const
{
    return new ExponentialDistribution(lambda);
}

/**
 * @brief Convenience function to set \f$ \beta = \frac{1}{\lambda} \f$
 * @param value
 */

void ExponentialDistribution::setBeta(MCfloat value) {
    setLambda(1./value);
}

/**
 * @brief Sets the rate \f$ \lambda \f$
 * @param value
 */

void ExponentialDistribution::setLambda(MCfloat value) {
    lambda = value;
    reconstructDistribution();
}

MCfloat ExponentialDistribution::spin() const {
    return distribution(*mt);
}




// Sech2 (Logistic) distribution

Sech2Distribution::Sech2Distribution(MCfloat mean, MCfloat scale, BaseObject *parent) :
    AbstractDistribution(parent)
{
    this->mean = mean;
    this->scale = scale;
}

MCfloat Sech2Distribution::spin() const {
    MCfloat p;
    do {
        p = uniform_01<MCfloat>()(*mt);
    }
    while(p == 0);
    return  mean + scale*log(p/(1.-p));
}

void Sech2Distribution::setScale(MCfloat value) {
    scale = value;
}

void Sech2Distribution::setMean(MCfloat value) {
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

void Sech2Distribution::setFWHM(MCfloat value) {
    setScale(value/(4*log(1 + root_two<MCfloat>())));
}
