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
    AbstractDistribution(int seed);
    virtual ~AbstractDistribution();

    /**
     * @brief Generates a new random number.
     * @return The newly generated random number.
     */
    virtual double spin() = 0;
    virtual void reconstructGenerator() {}
    void setSeed(int seed);

protected:
    mt19937* mt;
    bool usingInternalMt;
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

typedef variate_generator<mt19937&, normal_distribution<double> > normalGenerator;

/**
 * @brief Normal (Gaussian) distribution \f$ f(x) = (2\pi \sigma)^{-\frac{1}{2}}
 * \exp \left( -\frac{(x-\mu)^2}{2 \sigma^2} \right) \f$
 */

class NormalDistribution : public AbstractDistribution
{
public:
    NormalDistribution(double mean, double sigma, mt19937* mt);
    NormalDistribution(double mean, double sigma, int seed);
    ~NormalDistribution();

    inline double spin() {return (*generator)();}
    void setMean(double value);
    void setSigma(double value);
    void setFWHM(double value);

private:
    void commonConstructor(double mean, double sigma);
    void reconstructGenerator();

    normalGenerator *generator;
    double mean, sigma;
};




// Uniform disribution

typedef variate_generator<mt19937&, uniform_real_distribution<double> > uniformGenerator;

/**
 * @brief Uniform real distribution
 */

class UniformDistribution : public AbstractDistribution
{
public:
    UniformDistribution(double min, double max, mt19937* mt);
    UniformDistribution(double min, double max, int seed);
    ~UniformDistribution();

    inline double spin() {return (*generator)();}

private:    
    void commonConstructor(double min, double max);
    void reconstructGenerator();

    uniformGenerator *generator;
    double min, max;
};




// Exponential distribution

typedef variate_generator<mt19937&, exponential_distribution<double> > exponentialGenerator;

/**
 * @brief Exponential distribution \f$ f(x) = \lambda \exp ( -\lambda x) \f$
 */

class ExponentialDistribution : public AbstractDistribution
{
public:
    ExponentialDistribution(double lambda, mt19937* mt);
    ExponentialDistribution(double lambda, int seed);
    ~ExponentialDistribution();

    inline double spin() {return (*generator)();}

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
 */

class Sech2Distribution : public AbstractDistribution
{
public:
    Sech2Distribution(double mean, double scale, mt19937* mt);
    Sech2Distribution(double mean, double scale, int seed);
    ~Sech2Distribution();

    double spin();
    void setMean(double value);
    void setScale(double value);
    void setFWHM(double value);

private:
    void commonConstructor(double mean, double scale);

    UniformDistribution *uRandom;
    double mean, scale;
};


#endif // DISTRIBUTIONS_H
