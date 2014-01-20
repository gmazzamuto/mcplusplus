#include "simulation.h"
#include "distributions.h"
#include "costhetagenerator.h"
#include "psigenerator.h"

#include <boost/math/special_functions/sign.hpp>

using namespace boost;
using namespace boost::math;

Simulation::Simulation()
{
    sample = NULL;
    source = NULL;
    reset();
}

void Simulation::reset() {
    totalWalkers = 0;
    transmitted = 0;
    reflected = 0;
    ballistic = 0;
    backreflected = 0;
}

void Simulation::setTotalWalkers(int N) {
    totalWalkers = N;
}

void Simulation::setSample(Sample *sample) {
    this->sample = sample;
}

void Simulation::setSource(Source *source) {
    this->source = source;
}

void Simulation::run() {
    mt19937 *mt = new mt19937(0);
    double totalThickness = sample->totalThickness();

    int n = 0;

    ExponentialDistribution *stepLength = new ExponentialDistribution(mt);
    IsotropicCosThetaGenerator *isotrCosTheta = new IsotropicCosThetaGenerator(mt);
    PsiGenerator *randomPsi = new PsiGenerator(mt);

    while(n < totalWalkers) {
        Walker *walker = source->constructWalker();

        while(1) {
            double step = stepLength->spin();
            double cosTheta = isotrCosTheta->spin();
            double sinTheta = sqrt(1-pow(cosTheta,2));
            double psi = randomPsi->spin();
            double cosPsi = cos(psi);
            double sinPsi = sin(psi);

            if(fabs(walker->k0[2]) > 0.999999) {
                walker->k1[0] = sinTheta*cosPsi;
                walker->k1[1] = sinTheta*sinPsi;
                walker->k1[2] = cosTheta*sign<double>(walker->k0[2]);
            }
            else { //check and set some intermediate temporary variables --> like sqrt(1-pow(walker->k1[3],2))
                walker->k1[0] = (sinTheta*(walker->k1[0]*walker->k1[2]*cosPsi - walker->k1[1]*sinPsi))/sqrt(1-pow(walker->k1[3],2)) + cosTheta*walker->k1[0];
                walker->k1[1] = (sinTheta*(walker->k1[1]*walker->k1[2]*cosPsi + walker->k1[0]*sinPsi))/sqrt(1-pow(walker->k1[3],2)) + cosTheta*walker->k1[1];
                walker->k1[2] = -sinTheta*cosPsi*sqrt(1-pow(walker->k1[3],2)) + cosTheta*walker->k1[2];
            }

            walker->r1[0] = walker->r0[0] + step*walker->k1[0];
            walker->r1[1] = walker->r0[1] + step*walker->k1[1];
            walker->r1[2] = walker->r0[2] + step*walker->k1[2];

            if(walker->r1[2] > totalThickness) {
                transmitted++;
                break;
            }

            if(walker->r1[2] < 0) {
                reflected++;
                break;
            }

            memcpy(walker->k0,walker->k1,3*sizeof(double));
            memcpy(walker->k0,walker->k1,3*sizeof(double));

            walker->walkDistance += step;
            walker->walkTime += step; //FIXME speed of light
        }

        n++;
        delete walker;
    }
}
