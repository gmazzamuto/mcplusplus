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
    DeltaDistribution(MCfloat center, BaseObject *parent=NULL);

    void setCenter(MCfloat val);
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
    NormalDistribution(MCfloat mean, MCfloat sigma, BaseObject *parent = NULL);
    virtual ~NormalDistribution();

    void setMean(MCfloat value);
    void setSigma(MCfloat value);
    void setFWHM(MCfloat value);
    virtual MCfloat spin() const;

private:
    void reconstructDistribution();
    virtual BaseObject* clone_impl() const;

    normal_distribution<MCfloat> *distribution;
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
    UniformDistribution(MCfloat min, MCfloat max, BaseObject *parent=NULL);

    virtual MCfloat spin() const;
    MCfloat spinOpen() const;

private:    
    void reconstructDistribution();
    virtual BaseObject* clone_impl() const;

    uniform_real_distribution<MCfloat> distribution;
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
    ExponentialDistribution(MCfloat lambda, BaseObject *parent=NULL);

    void setBeta(MCfloat value);
    void setLambda(MCfloat value);
    MCfloat spin() const;

private:
    void reconstructDistribution();
    virtual BaseObject* clone_impl() const;

    exponential_distribution<MCfloat> distribution;
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
    Sech2Distribution(MCfloat mean, MCfloat scale, BaseObject* parent=NULL);

    void setMean(MCfloat value);
    void setScale(MCfloat value);
    void setFWHM(MCfloat value);
    MCfloat spin() const;

private:
    virtual BaseObject* clone_impl() const;
    MCfloat mean, scale;
};

}
#endif // DISTRIBUTIONS_H
