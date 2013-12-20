#include <stdio.h>
#include <boost/random/variate_generator.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/discrete_distribution.hpp>
#include <boost/random/uniform_real.hpp>

int main(int argc, char *argv[])
{
    int seed = 0;

    double sigma = 0.5;

    boost::variate_generator<boost::mt19937, boost::normal_distribution<> >
            generator(boost::mt19937(seed),
                      boost::normal_distribution<>(0,sigma));


    for (int i = 0; i < 1000; ++i) {
        printf("%f\n",generator());
    }

    return 0;
}
