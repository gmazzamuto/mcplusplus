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
    upperZBoundaries = NULL;
    currentLayer = 0;
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
    nLayers = sample->nLayers();
    double totalThickness = sample->totalThickness();
    upperZBoundaries = sample->zBoundaries();

    int n = 0;

    ExponentialDistribution *stepLength = new ExponentialDistribution(this);
    CosThetaGenerator *deflCosine = new CosThetaGenerator(0,this); // I set g=0 without any particular reason
    IsotropicPsiGenerator *randomPsi = new IsotropicPsiGenerator(this);

    while(n < totalWalkers) {
        currentLayer = 0;

        Walker *walker = source->constructWalker();
        walker->nInteractions.insert(walker->nInteractions.begin(),nLayers,0);

        while(1) {
            updateCurrentLayer(walker->r0);
            printf("%d\t",currentLayer);
            printf("%lf\t%lf\t%lf\n", walker->r0[0], walker->r0[1], walker->r0[2]);

            stepLength->setBeta(sample->material(currentLayer)->ls); //should we check if currentLayer has changed?
            deflCosine->setg(sample->material(currentLayer)->g);

            double length = stepLength->spin();
            double cosTheta = deflCosine->spin();
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

        updateCurrentLayer(walker->r0);
        printf("walker reached layer %d:\n",currentLayer);
        printf("%lf\t%lf\t%lf\n", walker->r0[0], walker->r0[1], walker->r0[2]);
        n++;
        delete walker;
    }
}

void Simulation::updateCurrentLayer(double *r0) {
    currentLayer = layerAt(r0);
}

/**
 * @brief Determines the layer index for the given position
 * @param r0
 * @return The index of the layer containing the point r0
 *
 * Interfaces are considered to belong to the preceding layer
 */

int Simulation::layerAt(double *r0) const {
    double z = r0[2];

    if(z < upperZBoundaries->at(currentLayer)) { //search left
        for (int i = currentLayer; i >= 0; i--) {
            if(r0[2] > upperZBoundaries->at(i))
                return i+1;
        }
        return 0;
    }
    else //search right
    {
        for (int i = currentLayer; i < nLayers+1; ++i) {
            if(z <= upperZBoundaries->at(i))
                return i;
        }
        return nLayers+1;
    }
}
