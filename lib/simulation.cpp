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
    layer0 = 0;
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
    double totalThickness = sample->totalThickness(); //we shouldn't need this (totalTickness is the last zBoundary)
    upperZBoundaries = sample->zBoundaries();

    int n = 0;

    ExponentialDistribution *stepLength = new ExponentialDistribution(this);
    CosThetaGenerator *deflCosine = new CosThetaGenerator(0,this); // I set g=0 without any particular reason
    IsotropicPsiGenerator *randomPsi = new IsotropicPsiGenerator(this);

    while(n < totalWalkers) {
        Walker *walker = source->constructWalker();
        walker->nInteractions.insert(walker->nInteractions.begin(),nLayers+2,0);

        layer0 = layerAt(walker->r0);
        printf("%d\t",layer0);
        printf("%lf\t%lf\t%lf\n", walker->r0[0], walker->r0[1], walker->r0[2]);

        while(1) {

            stepLength->setBeta(sample->material(layer0)->ls);
            deflCosine->setg(sample->material(layer0)->g);

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

            move(walker,length); //since I think that move() is conceptually the right place to increment walkTime it is convenient to pass the length as well
                                 //or we might let length to be a private Simulation member, and update its value accordingly with its remaining portion when a reflection takes place

            printf("%d\t",layer0);
            printf("%lf\t%lf\t%lf\n", walker->r0[0], walker->r0[1], walker->r0[2]);


            if(layer0 == nLayers + 1) {// +1, right?
                transmitted++;
                break;
            }

            if(layer0 == 0) {
                reflected++;
                break;
            }
        }

        printf("walker reached layer %d:\n",layer0);
        n++;
        delete walker;
    }
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

    if(z < upperZBoundaries->at(layer0)) { //search left
        for (int i = layer0; i >= 0; i--) {
            if(r0[2] > upperZBoundaries->at(i))
                return i+1;
        }
        return 0;
    }
    else //search right
    {
        for (int i = layer0; i < nLayers+1; ++i) {
            if(z <= upperZBoundaries->at(i))
                return i;
        }
        return nLayers+1;
    }
}

void Simulation::move(Walker *walker, double length) {
    layer1 = layerAt(walker->r1);
    if(layer1 == layer0) {
        memcpy(walker->r0,walker->r1,3*sizeof(double));
        memcpy(walker->k0,walker->k1,3*sizeof(double));

        walker->walkTime += length/sample->material(layer0)->v;
        walker->nInteractions[layer0]++; //not sure this is the right place to increment the interactions counter as well...
        return;
    }
    else {
        double zBoundary=0;
        layer1 = layer0 + sign(walker->k0[2]);
        zBoundary = upperZBoundaries->at(min(layer0,layer1));

        double t = (zBoundary - walker->r0[2]) / walker->k1[2];
        double intersection[3];
        for (int i = 0; i < 3; ++i) {
            intersection[i] = walker->r0[i] + walker->k1[i]*t;
        }

        memcpy(walker->r0,intersection,3*sizeof(double)); //move to interface, r1 is now meaningless (should we set it to NULL?)
        walker->walkTime += t/sample->material(layer0)->v;

        //so we updated r0, now it's time to update k0

        double n0 = sample->material(layer0)->n;
        double n1 = sample->material(layer1)->n;

        if (n0 == n1) {
            layer0 = layer1; //leave k0 as it is, update layer
            return;
        }
        else {
            double sinTheta1 = sqrt(1 - pow(walker->k0[2],2));
            double sinTheta2 = n0*sinTheta1/n1;

            if(sinTheta2 > 1)
                reflect(walker);
            else {
                //calculate the probability r(Theta1,n0,n1) of being reflected
                double cThetaSum, cThetaDiff; //cos(Theta1 + Theta2) and cos(Theta1 - Theta2)
                double sThetaSum, sThetaDiff; //sin(Theta1 + Theta2) and sin(Theta1 - Theta2)

                double cosTheta1 = walker->k0[2];
                double cosTheta2 = sqrt(1 - pow(sinTheta2,2));

                cThetaSum = cosTheta1*cosTheta2 - sinTheta1*sinTheta2;
                cThetaDiff = cosTheta1*cosTheta2 + sinTheta1*sinTheta2;
                sThetaSum = sinTheta1*cosTheta2 + cosTheta1*sinTheta2;
                sThetaDiff = sinTheta1*cosTheta2 - cosTheta1*sinTheta2;
                double r = 0.5*sThetaDiff*sThetaDiff*(cThetaDiff*cThetaDiff+cThetaSum*cThetaSum)/(sThetaSum*sThetaSum*cThetaDiff*cThetaDiff);

                double xi = uniform_01<double>()(*mt);

                if(xi <= r)
                    reflect(walker); //we come back to run() without keeping track of the extra unused step length
                refract(walker);
            }
        }
    }
}

void Simulation::reflect(Walker *walker) {
    layer1 = layer0;
    walker->k0[2] = -walker->k0[2];
}

void Simulation::refract(Walker *walker) {
    double n0 = sample->material(layer0)->n; //I have already defined those quantities. should we pass them as arguments?
    double n1 = sample->material(layer1)->n; //
    double sinTheta1 = sqrt(1 - pow(walker->k0[2],2)); //

    walker->k0[0] = n0*walker->k0[0]/n1;
    walker->k0[1] = n0*walker->k0[1]/n1;
    walker->k0[2] = sqrt(1 - pow(n0*sinTheta1/n1,2));
}
