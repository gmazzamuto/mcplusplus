#ifndef DISTRIBUTIONS_H
#define DISTRIBUTIONS_H

#include <boost/random.hpp>
#include <math.h>

using namespace boost;
using namespace boost::random;

// Abstract distribution

/**
 * @brief Base distribution class. Provides a common interface to handle various distributions.
 */

class AbstractDistribution
{
public:
    AbstractDistribution(mt19937* mt = NULL);
    virtual ~AbstractDistribution();

    /**
     * @brief Generate a new random number.
     * @return The newly generated random number.
     */
    virtual double spin() = 0;

protected:
    mt19937* mt;
};




// Dirac Delta distribution

/**
 * @brief Dirac Delta distribution \f$ f(x) = \delta (x) \f$
 */

class DeltaDistribution : public AbstractDistribution
{
public:
    DeltaDistribution(double center);
    inline double spin() {return x0;}

private:
    double x0;
};




// Normal (Gaussian) distribution

typedef variate_generator<mt19937, normal_distribution<double> > normalGenerator;

/**
 * @brief Normal (Gaussian) distribution \f$ f(x) = (2\pi \sigma)^{-\frac{1}{2}} \exp \left( -\frac{(x-\mu)^2}{2 \sigma^2} \right) \f$
 */

class NormalDistribution : public AbstractDistribution
{
public:
    NormalDistribution(mt19937* mt, double mean, double sigma);
    ~NormalDistribution();

    inline double spin() {return (*generator)();}
    void setMean(double value);
    void setSigma(double value);
    void setFWHM(double value);

private:
    void reconstructGenerator();

    normalGenerator *generator;
    double mean, sigma;
};




// Uniform disribution

typedef variate_generator<mt19937, uniform_real_distribution<double> > uniformGenerator;

/**
 * @brief Uniform real distribution
 */

class UniformDistribution : public AbstractDistribution
{
public:
    UniformDistribution(mt19937* mt, double min = 0, double max = 1);
    ~UniformDistribution();

    inline double spin() {return (*generator)();}

private:
    uniformGenerator *generator;
};




// Exponential distribution

typedef variate_generator<mt19937, exponential_distribution<double> > exponentialGenerator;

/**
 * @brief Exponential distribution \f$ f(x) = \lambda \exp ( -\lambda x) \f$
 */

class ExponentialDistribution : public AbstractDistribution
{
public:
    ExponentialDistribution(mt19937* mt, double lambda);
    ~ExponentialDistribution();

    inline double spin() {return (*generator)();}

private:
    exponentialGenerator *generator;
};




// Sech2 (Logistic) distribution

/**
 * @brief Hyperbolic secant square distribution \f$ f(x) = (4s)^{-1} \, \textup{sech}^2 \left( \frac{x - \mu}{2s} \right) \f$
 *
 * See <a href="http://en.wikipedia.org/wiki/Logistic_distribution">Wikpiedia</a> for further reference on quantile function.
 */

class Sech2Distribution : public AbstractDistribution
{
public:
    Sech2Distribution(mt19937* mt, double mean, double scale);
    ~Sech2Distribution();

    double spin();

private:
    UniformDistribution *uRandom;
    double mean, scale;
};


#endif // DISTRIBUTIONS_H
