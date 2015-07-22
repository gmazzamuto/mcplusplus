/*
  This file is part of MCPlusPlus.

  MCPlusPlus is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Foobar is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with MCPlusPlus.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <MCPlusPlus/distributions.h>

#include <boost/math/constants/constants.hpp>

using namespace boost::math::constants;
using namespace MCPP;



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

MCfloat DeltaDistribution::spin() const {
    return x0;
}

BaseObject * DeltaDistribution::clone_impl() const
{
    return new DeltaDistribution(x0);
}

void DeltaDistribution::describe_impl() const
{
    logMessage("x0 = %f", x0);
}

bool DeltaDistribution::sanityCheck_impl() const
{
    return true;
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
    distribution = new normal_distribution<MCfloat>(mean,sigma);
}

BaseObject *NormalDistribution::clone_impl() const
{
    return new NormalDistribution(mean,sigma);
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
 */

void NormalDistribution::setFWHM(double value) {
    setSigma(value/(2*root_ln_four<MCfloat>()));
}

MCfloat NormalDistribution::spin() const {
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

ExponentialDistribution::ExponentialDistribution(double lambda, BaseObject *parent) :
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

MCfloat ExponentialDistribution::spin() const {
    return distribution(*mt);
}




// Sech2 (Logistic) distribution

Sech2Distribution::Sech2Distribution(double mean, double scale, BaseObject *parent) :
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

BaseObject *Sech2Distribution::clone_impl() const
{
    return new Sech2Distribution(mean,scale);
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
 */

void Sech2Distribution::setFWHM(double value) {
    setScale(value/(4*log(1 + root_two<MCfloat>())));
}
