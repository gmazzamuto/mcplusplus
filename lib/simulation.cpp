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
    _sample = NULL;
    source = NULL;
    upperZBoundaries = NULL;
    layer0 = 0;
    trajectoryPoints = new std::vector<vector<double>*>();
    saveTrajectory = false;
    snellReflectionsEnabled = true;
    reset();
}

Simulation::~Simulation() {
    delete trajectoryPoints;
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

/**
 * @brief Sets the kind of sample to be simulated. The simulation is
 * automatically set as the sample's parent.
 * @param sample
 */

void Simulation::setSample(Sample *sample) {
    sample->setParent(this);
    _sample = sample;
}

const Sample* Simulation::sample() const {
    return _sample;
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

void Simulation::setSnellReflectionsEnabled(bool enable)
{
    snellReflectionsEnabled = enable;
}

/**
 * @brief Runs the simulation
 *
 * \pre The RNG has to be valid (see BaseRandom)
 */

void Simulation::run() {
    nLayers = _sample->nLayers();
    upperZBoundaries = _sample->zBoundaries();

    int n = 0;

    ExponentialDistribution *stepLength = new ExponentialDistribution(this);
    CosThetaGenerator *deflCosine = new CosThetaGenerator(0,this); // I set g=0 without any particular reason
    IsotropicPsiGenerator *randomPsi = new IsotropicPsiGenerator(this);

    while(n < totalWalkers) {
        currentTrajectory = new std::vector<double>();
        layer0 = 0;

        Walker *walker = source->constructWalker();
        walker->nInteractions.insert(walker->nInteractions.begin(),nLayers+2,0);

        layer0 = layerAt(walker->r0); //updates also onInterface flag

        memcpy(walker->k1,walker->k0,3*sizeof(double));
        onInterface = true; //treat newly generated walker as it were on an interface, i.e. do not scatter

        saveTrajectoryPoint(walker->r0);

#ifdef DEBUG_TRAJECTORY
        printf("%d\t",layer0);
        printf("%lf\t%lf\t%lf\t\t%lf", walker->r0[0], walker->r0[1], walker->r0[2],walker->k0[2]);
#endif

        double length;
        while(1) {
            //spin k1 (i.e. scatter) only if the material is scattering
            //and we're not on an interface, use the old k1 otherwise
            if(_sample->material(layer0)->ls > 0) {
                stepLength->setBeta(_sample->material(layer0)->ls);
                length = stepLength->spin();
                if(!onInterface) {
                    deflCosine->setg(_sample->material(layer0)->g);
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
                }
            }
            else { //no scattering
                length = std::numeric_limits<double>::infinity();
            }

            walker->r1[0] = walker->r0[0] + length*walker->k1[0];
            walker->r1[1] = walker->r0[1] + length*walker->k1[1];
            walker->r1[2] = walker->r0[2] + length*walker->k1[2];

#ifdef DEBUG_TRAJECTORY
            printf("\t%lf\n",walker->k1[2]);
#endif
            move(walker,length); //since I think that move() is conceptually the right place to increment walkTime it is convenient to pass the length as well
                                 //or we might let length to be a private Simulation member, and update its value accordingly with its remaining portion when a reflection takes place

#ifdef DEBUG_TRAJECTORY
            printf("%d\t",layer0);
            printf("%lf\t%lf\t%lf\t\t%lf", walker->r0[0], walker->r0[1], walker->r0[2],walker->k0[2]);
#endif
            saveTrajectoryPoint(walker->r0);

            if(layer0 == nLayers + 1) {
                bool diffuselyTransmitted = false;
                for (int i = 1; i <= nLayers; ++i) {
                    if(walker->nInteractions[i]) {
                        transmitted++;
                        diffuselyTransmitted = true;
                        break;
                    }
                }
                if(!diffuselyTransmitted)
                    ballistic++;
                break;
            }

            if(layer0 == 0) {
                bool diffuselyReflected = false;
                for (int i = 1; i <= nLayers; ++i) {
                    if(walker->nInteractions[i]) {
                        reflected++;
                        diffuselyReflected = true;
                        break;
                    }
                }
                if(!diffuselyReflected)
                    backreflected++;
                break;
            }
        }

        if(saveTrajectory)
            trajectoryPoints->push_back(currentTrajectory);

#ifdef DEBUG_TRAJECTORY
        printf("\nwalker reached layer %d\n",layer0);
#endif
        n++;
        delete walker;
    }

    printf("transmitted: %d\n",transmitted);
    printf("reflected: %d\n",reflected);
    printf("ballistic: %d\n",ballistic);
    printf("back-reflected: %d\n",backreflected);
}

/**
 * @brief Determines the layer index for the given position
 * @param r0
 * @return The index of the layer containing the point r0
 *
 * Interfaces are considered to belong to the preceding layer
 */

int Simulation::layerAt(double *r0) {
    onInterface = false;
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
            if(z < upperZBoundaries->at(i))
                return i;
            if(z == upperZBoundaries->at(i)) {
                onInterface = true;
                return i;
            }
        }
        return nLayers+1;
    }
}

void Simulation::move(Walker *walker, double length) {
    layer1 = layerAt(walker->r1);
    if(layer1 == layer0) {
        memcpy(walker->r0,walker->r1,3*sizeof(double));
        memcpy(walker->k0,walker->k1,3*sizeof(double));

        walker->walkTime += length/_sample->material(layer0)->v;
        walker->nInteractions[layer0]++; //not sure this is the right place to increment the interactions counter as well...
        return;
    }

    //handle interface

    double zBoundary=0;
    layer1 = layer0 + sign(walker->k1[2]);
    zBoundary = upperZBoundaries->at(min(layer0,layer1));

    double t = (zBoundary - walker->r0[2]) / walker->k1[2];
    double intersection[3];
    for (int i = 0; i < 3; ++i) {
        intersection[i] = walker->r0[i] + walker->k1[i]*t;
    }

    memcpy(walker->r0,intersection,3*sizeof(double)); //move to interface, r1 is now meaningless (should we set it to NULL?)
    onInterface = true;
    walker->walkTime += t/_sample->material(layer0)->v;

    //so we updated r0, now it's time to update k0

    double n0 = _sample->material(layer0)->n;
    double n1 = _sample->material(layer1)->n;

    if (n0 == n1) {
#ifdef DEBUG_TRAJECTORY
        printf("interface with same n...\n");
#endif
        layer0 = layer1;
        return;
    }

    //handle reflection and refraction

    double sinTheta1 = sqrt(1 - pow(walker->k1[2],2));
    double sinTheta2 = n0*sinTheta1/n1;

    if(sinTheta2 > 1) {
#ifdef DEBUG_TRAJECTORY
        printf("TIR ");
#endif
        reflect(walker);
    }
    else {
        double r;
        if(snellReflectionsEnabled)
        {
            //calculate the probability r(Theta1,n0,n1) of being reflected
            double cThetaSum, cThetaDiff; //cos(Theta1 + Theta2) and cos(Theta1 - Theta2)
            double sThetaSum, sThetaDiff; //sin(Theta1 + Theta2) and sin(Theta1 - Theta2)

            double cosTheta1 = fabs(walker->k1[2]);
            double cosTheta2 = sqrt(1 - pow(sinTheta2,2));

            cThetaSum = cosTheta1*cosTheta2 - sinTheta1*sinTheta2;
            cThetaDiff = cosTheta1*cosTheta2 + sinTheta1*sinTheta2;
            sThetaSum = sinTheta1*cosTheta2 + cosTheta1*sinTheta2;
            sThetaDiff = sinTheta1*cosTheta2 - cosTheta1*sinTheta2;
            r = 0.5*sThetaDiff*sThetaDiff*(cThetaDiff*cThetaDiff+cThetaSum*cThetaSum)/(sThetaSum*sThetaSum*cThetaDiff*cThetaDiff);
        }
        else
            r = 0;

        double xi = uniform_01<double>()(*mt);

        if(xi <= r) {
            reflect(walker); //we come back to run() without keeping track of the extra unused step length
            return;
        }
        refract(walker);
    }
}

void Simulation::reflect(Walker *walker) {
#ifdef DEBUG_TRAJECTORY
    printf("reflect ...\n");
#endif
    layer1 = layer0;
    walker->k1[2] *= -1; //flip k1 along z
}

void Simulation::refract(Walker *walker) {
#ifdef DEBUG_TRAJECTORY
    printf("refract ...\n");
#endif
    double n0 = _sample->material(layer0)->n; //I have already defined those quantities. should we pass them as arguments?
    double n1 = _sample->material(layer1)->n; //
    double sinTheta1 = sqrt(1 - pow(walker->k1[2],2)); //

    walker->k1[0] = n0*walker->k1[0]/n1;
    walker->k1[1] = n0*walker->k1[1]/n1;
    walker->k1[2] = sqrt(1 - pow(n0*sinTheta1/n1,2));
    layer0=layer1;
}

void Simulation::saveTrajectoryPoint(double *point) {
    if(!saveTrajectory)
        return;
    for (int i = 0; i < 3; ++i) {
        currentTrajectory->push_back(point[i]);
    }
}

void Simulation::setSaveTrajectoryEnabled(bool enabled) {
    saveTrajectory = enabled;
}

/**
 * @brief Returns the trajectory of the last simulated walker
 * @return
 */

const vector< vector <double>*> *Simulation::trajectories() const {
    return trajectoryPoints;
}
