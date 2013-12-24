#include "distributions.h"


AbstractDistribution::AbstractDistribution(mt19937 *mt) {
    this->mt = mt;
}

AbstractDistribution::~AbstractDistribution() {}




DeltaDistribution::DeltaDistribution(double center) :
    AbstractDistribution()
{
    x0 = center;
}




NormalDistribution::NormalDistribution(mt19937 *mt, double mean, double sigma) :
    AbstractDistribution(mt)
{
    normal_distribution<double> normalDistribution(mean,sigma);
    generator = new normalGenerator(*mt,normalDistribution);
}

NormalDistribution::~NormalDistribution() {
    delete generator;
}




UniformDistribution::UniformDistribution(mt19937 *mt, double min, double max) :
    AbstractDistribution(mt)
{
    uniform_real_distribution<double> uniformRealDistribution(min,max);
    generator = new uniformGenerator(*mt,uniformRealDistribution);
}

UniformDistribution::~UniformDistribution() {
    delete generator;
}




ExponentialDistribution::ExponentialDistribution(mt19937 *mt, double lambda) :
    AbstractDistribution(mt)
{
   exponential_distribution<double> exponentialRealDistribution(lambda);
   generator = new exponentialGenerator(*mt,exponentialRealDistribution);
}

ExponentialDistribution::~ExponentialDistribution() {
    delete generator;
}




Sech2Distribution::Sech2Distribution(mt19937 *mt, double mean, double scale) :
    AbstractDistribution()
{
    this->mean = mean;
    this->scale = scale;
    uRandom = new UniformDistribution(mt,0,1);
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
