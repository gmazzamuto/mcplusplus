#include "distributions.h"


#include <boost/math/constants/constants.hpp>

using namespace boost::math::constants;


// Abstract distribution

AbstractDistribution::AbstractDistribution(mt19937 *mt) {
    this->mt = mt;
    usingInternalMt = false;
}

AbstractDistribution::AbstractDistribution(int seed) {
    mt = new mt19937(seed);
    usingInternalMt = true;
}

AbstractDistribution::~AbstractDistribution() {
    if(usingInternalMt)
        delete mt;
}

void AbstractDistribution::setSeed(int seed) {
    if(!usingInternalMt)
        return;
    if(mt != NULL)
        delete mt;
    mt = new mt19937(seed);
    reconstructGenerator();
}




// Dirac Delta distribution

DeltaDistribution::DeltaDistribution(double center) :
    AbstractDistribution()
{
    x0 = center;
}




// Normal (Gaussian) distribution

NormalDistribution::NormalDistribution(double mean, double sigma, mt19937 *mt) :
    AbstractDistribution(mt)
{
    commonConstructor(mean, sigma);
}

NormalDistribution::NormalDistribution(double mean, double sigma, int seed) :
    AbstractDistribution(seed)
{
    commonConstructor(mean, sigma);
}

void NormalDistribution::commonConstructor(double mean, double sigma) {
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

UniformDistribution::UniformDistribution(double min, double max, mt19937 *mt) :
    AbstractDistribution(mt)
{
    commonConstructor(min,max);
}

UniformDistribution::UniformDistribution(double min, double max, int seed) :
    AbstractDistribution(seed)
{
    commonConstructor(min,max);
}

UniformDistribution::~UniformDistribution() {
    delete generator;
}

void UniformDistribution::commonConstructor(double min, double max) {
    this->min = min;
    this->max = max;
    reconstructGenerator();
}

void UniformDistribution::reconstructGenerator() {
    if(generator != NULL)
        delete generator;
    uniform_real_distribution<double> uniformRealDistribution(min,max);
    generator = new uniformGenerator(*mt,uniformRealDistribution);
}




// Exponential distribution

ExponentialDistribution::ExponentialDistribution(double lambda, mt19937 *mt) :
    AbstractDistribution(mt)
{
    commonConstructor(lambda);
}

ExponentialDistribution::ExponentialDistribution(double lambda, int seed) :
    AbstractDistribution(seed)
{
    commonConstructor(lambda);
}

void ExponentialDistribution::commonConstructor(double lambda) {
    this->lambda = lambda;
    reconstructGenerator();
}

ExponentialDistribution::~ExponentialDistribution() {
    delete generator;
}

void ExponentialDistribution::reconstructGenerator() {
    exponential_distribution<double> exponentialRealDistribution(lambda);
    generator = new exponentialGenerator(*mt,exponentialRealDistribution);
}




// Sech2 (Logistic) distribution

Sech2Distribution::Sech2Distribution(double mean, double scale, mt19937 *mt) :
    AbstractDistribution(mt)
{
    commonConstructor(mean, scale);
}

Sech2Distribution::Sech2Distribution(double mean, double scale, int seed) :
    AbstractDistribution(seed)
{
    commonConstructor(mean, scale);
}

void Sech2Distribution::commonConstructor(double mean, double scale) {
    this->mean = mean;
    this->scale = scale;
    uRandom = new UniformDistribution(0,1,mt);
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
