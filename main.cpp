#include <stdio.h>
#include <boost/random/variate_generator.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/discrete_distribution.hpp>
#include <boost/random/uniform_real_distribution.hpp>

int main(int argc, char *argv[])
{
    int seed = 0;

    double sigma = 0.5;

    boost::random::variate_generator<boost::random::mt19937, boost::random::normal_distribution<> >
            normalGenerator(boost::random::mt19937(seed),
                      boost::random::normal_distribution<>(0,sigma));


    boost::random::variate_generator<boost::random::mt19937, boost::random::uniform_real_distribution<double> >
            uniformGenerator(boost::random::mt19937(seed),boost::random::uniform_real_distribution<double>(0,1));


    for (int i = 0; i < 1000; ++i) {
        printf("%f %f\n", uniformGenerator(), normalGenerator());
    }

    return 0;
}
