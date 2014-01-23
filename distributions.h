#ifndef DISTRIBUTIONS_H
#define DISTRIBUTIONS_H

#include <boost/random.hpp>
#include <math.h>
#include "baseobject.h"

using namespace boost;
using namespace boost::random;

/**
  * \defgroup Distributions
  * \brief Classes providing random distributions
  */

// Abstract distribution

/**
 * @brief Base distribution class. Provides a common interface to handle various
 * distributions.
 *
 * This class is a wrapper around boost::variate_generator using a boost::mt19937
 * random number generator. By using the appropriate constructor this class can
 * use an external instance of the RNG or, alternatively, instantiate its own
 * internal RNG with the given seed.
 *
 * A new random number can be drawn by calling the spin() method which must be
 * reimplemented by derived classes.
 *
 * \ingroup Distributions
 */

class AbstractDistribution : public BaseObject
{
public:
    AbstractDistribution(BaseObject *parent=NULL);

    /**
     * @brief Generates a new random number.
     * @return The newly generated random number.
     */
    virtual double spin() = 0;
    virtual void reset();

protected:
    virtual void reconstructGenerator();

private:
    void setSeed_impl();
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
    double spin() {return x0;}

private:
    double x0;
};




// Normal (Gaussian) distribution

typedef variate_generator<mt19937&, normal_distribution<double> > normalGenerator;

/**
 * @brief Normal (Gaussian) distribution \f$ f(x) = (2\pi \sigma)^{-\frac{1}{2}}
 * \exp \left( -\frac{(x-\mu)^2}{2 \sigma^2} \right) \f$
 * \ingroup Distributions
 */

class NormalDistribution : public AbstractDistribution
{
public:
    NormalDistribution(double mean, double sigma, BaseObject *parent = NULL);
    ~NormalDistribution();

    double spin();
    void setMean(double value);
    void setSigma(double value);
    void setFWHM(double value);

private:
    void reconstructGenerator();

    normalGenerator *generator;
    double mean, sigma;
};




// Uniform disribution

typedef variate_generator<mt19937&, uniform_real_distribution<double> > uniformGenerator;

/**
 * @brief Uniform real distribution
 * \ingroup Distributions
 */

class UniformDistribution : public AbstractDistribution
{
public:
    UniformDistribution(double min, double max, BaseObject *parent=NULL);
    ~UniformDistribution();

    virtual double spin();

private:    
    void reconstructGenerator();

    uniformGenerator *generator;
    double min, max;
};




// Exponential distribution

typedef variate_generator<mt19937&, exponential_distribution<double> > exponentialGenerator;

/**
 * @brief Exponential distribution \f$ f(x) = \lambda \exp ( -\lambda x) \f$
 * \ingroup Distributions
 */

class ExponentialDistribution : public AbstractDistribution
{
public:
    ExponentialDistribution(BaseObject *parent=NULL);
    ExponentialDistribution(double lambda, BaseObject *parent);
    ~ExponentialDistribution();

    void setLamda(double value);
    double spin();

private:
    void commonConstructor(double lambda);
    void reconstructGenerator();

    exponentialGenerator *generator;
    double lambda;
};




// Sech2 (Logistic) distribution

/**
 * @brief Hyperbolic secant square distribution \f$ f(x) = (4s)^{-1} \,
 * \textup{sech}^2 \left( \frac{x - \mu}{2s} \right) \f$
 *
 * See <a href="http://en.wikipedia.org/wiki/Logistic_distribution">Wikpiedia</a>
 * for further reference on quantile function.
 * \ingroup Distributions
 */

class Sech2Distribution : public UniformDistribution
{
public:
    Sech2Distribution(double mean, double scale, BaseObject* parent=NULL);

    double spin();
    void setMean(double value);
    void setScale(double value);
    void setFWHM(double value);

private:
    double mean, scale;
};


#endif // DISTRIBUTIONS_H
