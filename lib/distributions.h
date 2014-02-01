#ifndef DISTRIBUTIONS_H
#define DISTRIBUTIONS_H

#include <math.h>
#include "baserandom.h"

/**
  * \defgroup Distributions
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
    virtual double spin() const = 0;

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
    double spin() const;

private:
    double x0;
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
    virtual double spin() const;

private:
    void reconstructDistribution();

    normal_distribution<double> *distribution;
    double mean, sigma;
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

    virtual double spin() const;
    double spinOpen() const;

private:    
    void reconstructDistribution();

    uniform_real_distribution<double> distribution;
    double min, max;
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
    ExponentialDistribution(double lambda, BaseObject *parent);

    void setBeta(double value);
    void setLambda(double value);
    double spin() const;

private:
    void reconstructDistribution();

    exponential_distribution<double> distribution;
    double lambda;
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
    double spin() const;

private:
    double mean, scale;
};

#endif // DISTRIBUTIONS_H
