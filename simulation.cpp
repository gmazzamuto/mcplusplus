#include "simulation.h"
#include "distributions.h"
#include "costhetagenerator.h"
#include "psigenerator.h"

#include <boost/math/special_functions/sign.hpp>

using namespace boost;
using namespace boost::math;

Simulation::Simulation(BaseObject *parent) :
    BaseRandom(parent)
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
}

void Simulation::setTotalWalkers(int N) {
    totalWalkers = N;
}

/**
 * @brief Sets the kind of sample to be simulated. The simulation is
 * automatically set as the sample's parent.
 * @param sample
 */

void Simulation::setSample(Sample *sample) {
    this->sample = sample;
    sample->setParent(this);
}

/**
 * @brief Sets the kind of source to be simulated. The simulation is
 * automatically set as the source's parent.
 * @param source
 */

void Simulation::setSource(Source *source) {
    this->source = source;
    source->setParent(this);
}

/**
 * @brief Runs the simulation
 *
 * \pre The RNG has to be valid (see BaseRandom)
 */

void Simulation::run() {
    int nLayers = sample->nLayers();
    double totalThickness = sample->totalThickness();

    int n = 0;

    ExponentialDistribution *stepLength = new ExponentialDistribution(this);
    IsotropicCosThetaGenerator *isotrCosTheta = new IsotropicCosThetaGenerator(this);
    IsotropicPsiGenerator *randomPsi = new IsotropicPsiGenerator(this);

    while(n < totalWalkers) {
        Walker *walker = source->constructWalker();
        walker->nInteractions.insert(walker->nInteractions.begin(),nLayers,0);

        while(1) {
            double length = stepLength->spin();
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
            else {
                double temp = sqrt(1-pow(walker->k0[2],2));
                walker->k1[0] = (sinTheta*(walker->k0[0]*walker->k0[2]*cosPsi - walker->k0[1]*sinPsi))/temp + cosTheta*walker->k0[0];
                walker->k1[1] = (sinTheta*(walker->k0[1]*walker->k0[2]*cosPsi + walker->k0[0]*sinPsi))/temp + cosTheta*walker->k0[1];
                walker->k1[2] = -sinTheta*cosPsi*temp + cosTheta*walker->k0[2];
            }

            walker->r1[0] = walker->r0[0] + length*walker->k1[0];
            walker->r1[1] = walker->r0[1] + length*walker->k1[1];
            walker->r1[2] = walker->r0[2] + length*walker->k1[2];

            memcpy(walker->r0,walker->r1,3*sizeof(double));
            memcpy(walker->k0,walker->k1,3*sizeof(double));

            walker->walkTime += length; //FIXME speed of light

            printf("%lf\t%lf\t%lf\n", walker->r0[0], walker->r0[1], walker->r0[2]);

            if(walker->r1[2] > totalThickness) {
                transmitted++;
                break;
            }

            if(walker->r1[2] < 0) {
                reflected++;
                break;
            }
            walker->nInteractions[0]++; //FIXME fix hardcoded layer index
        }

        n++;
        delete walker;
    }
}
