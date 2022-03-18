/*
  This file is part of MCPlusPlus.

  MCPlusPlus is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Mcplusplus is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with MCPlusPlus.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DISTRIBUTIONS_H
#define DISTRIBUTIONS_H

#include <cmath>
#include "baserandom.h"

namespace MCPP {

/**
  * \defgroup Distributions Distributions
  * \brief Classes providing random distributions
  */

// Abstract distribution

/**
 * @brief Base distribution class. Provides a common interface to handle various
 * distributions.
 *
 * This class joins a random number generator together with a random number
 * distribution, much like boost::variate_generator. As with every BaseRandom
 * object, the RNG used can be internal or the parent's.
 *
 * A new random number can be drawn by calling the spin() method which must be
 * reimplemented by derived classes.
 *
 * \ingroup Distributions
 */

class AbstractDistribution : public BaseRandom
{
public:
    AbstractDistribution(BaseObject *parent=NULL);

    void reset();
    /**
     * @brief Generates a new random variate.
     * @return The newly generated random variate.
     *
     * \pre The RNG has to be valid (see BaseRandom)
     */
    virtual MCfloat spin() const = 0;
    virtual BaseObject *clone_impl() const = 0;

protected:
    virtual void reconstructDistribution();

private:
    void setRNG_impl();
};




// Dirac Delta distribution

/**
 * @brief Dirac Delta distribution \f$ f(x) = \delta (x) \f$
 * \ingroup Distributions
 */

class DeltaDistribution : public AbstractDistribution
{
public:
    DeltaDistribution(double center, BaseObject *parent=NULL);

    void setCenter(double val);
    MCfloat spin() const;

private:
    MCfloat x0;
    virtual BaseObject* clone_impl() const;
    virtual void describe_impl() const;
    virtual bool sanityCheck_impl() const;
};




// Normal (Gaussian) distribution

/**
 * @brief Normal (Gaussian) distribution \f$ f(x) = (2\pi \sigma)^{-\frac{1}{2}}
 * \exp \left( -\frac{(x-\mu)^2}{2 \sigma^2} \right) \f$
 * \ingroup Distributions
 */

class NormalDistribution : public AbstractDistribution
{
public:
    NormalDistribution(double mean, double sigma, BaseObject *parent = NULL);
    virtual ~NormalDistribution();

    void setMean(double value);
    void setSigma(double value);
    void setFWHM(double value);
    virtual MCfloat spin() const;

private:
    void reconstructDistribution();
    virtual BaseObject* clone_impl() const;

    boost::random::normal_distribution<MCfloat> *distribution;
    MCfloat mean, sigma;
};




// Uniform disribution

/**
 * @brief Uniform real distribution in the interval \f$ [\textup{min,max}) \f$
 * \ingroup Distributions
 */

class UniformDistribution : public AbstractDistribution
{
public:
    UniformDistribution(double min, double max, BaseObject *parent=NULL);

    virtual MCfloat spin() const;
    MCfloat spinOpen() const;

private:
    void reconstructDistribution();
    virtual BaseObject* clone_impl() const;

    boost::random::uniform_real_distribution<MCfloat> distribution;
    MCfloat min, max;
};




// Exponential distribution

/**
 * @brief Exponential distribution \f$ f(x) = \lambda \exp ( -\lambda x) \f$
 * \ingroup Distributions
 */

class ExponentialDistribution : public AbstractDistribution
{
public:
    ExponentialDistribution(BaseObject *parent=NULL);
    ExponentialDistribution(double lambda, BaseObject *parent=NULL);

    void setBeta(double value);
    void setLambda(double value);
    MCfloat spin() const;

private:
    void reconstructDistribution();
    virtual BaseObject* clone_impl() const;

    boost::random::exponential_distribution<MCfloat> distribution;
    MCfloat lambda;
};




// Sech2 (Logistic) distribution

/**
 * @brief Hyperbolic secant square distribution \f$ f(x) = (4s)^{-1} \,
 * \textup{sech}^2 \left( \frac{x - \mu}{2s} \right) \f$
 *
 * See <a href="http://en.wikipedia.org/wiki/Logistic_distribution">Wikipedia</a>
 * for further reference on quantile function.
 * \ingroup Distributions
 */

class Sech2Distribution : public AbstractDistribution
{
public:
    Sech2Distribution(double mean, double scale, BaseObject* parent=NULL);

    void setMean(double value);
    void setScale(double value);
    void setFWHM(double value);
    MCfloat spin() const;

private:
    virtual BaseObject* clone_impl() const;
    MCfloat mean, scale;
};

}
#endif // DISTRIBUTIONS_H
