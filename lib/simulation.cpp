#include "simulation.h"
#include "distributions.h"
#include "costhetagenerator.h"
#include "psigenerator.h"
#include <signal.h>

#include <boost/math/special_functions/sign.hpp>

using namespace boost;
using namespace boost::math;

Simulation *mostRecentInstance=NULL;

void sigUsr1Handler(int signo) {
    Simulation *sim = mostRecentInstance;

    if(mostRecentInstance == NULL)
        return;

    sim->reportProgress();
}

Simulation::Simulation(BaseObject *parent) :
    BaseRandom(parent)
{
    _sample = NULL;
    source = NULL;
    upperZBoundaries = NULL;
    layer0 = 0;
    trajectoryPoints = new std::vector<vector<MCfloat>*>();
    saveTrajectory = false;
    fresnelReflectionsEnabled = true;
    reset();
    mostRecentInstance = this;
    signal(SIGUSR1,sigUsr1Handler);
}

Simulation::~Simulation() {
    delete trajectoryPoints;
    if(mostRecentInstance == this)
        mostRecentInstance = NULL;
}

void Simulation::reset() {
    _totalWalkers = 0;
    transmitted = 0;
    reflected = 0;
    ballistic = 0;
    backreflected = 0;
}

void Simulation::setTotalWalkers(int N) {
    _totalWalkers = N;
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

void Simulation::setFresnelReflectionsEnabled(bool enable)
{
    fresnelReflectionsEnabled = enable;
}

unsigned int Simulation::totalWalkers() const
{
    return _totalWalkers;
}

unsigned int Simulation::currentWalker() const
{
    return n;
}

/**
 * @brief Runs the simulation
 *
 * \pre The RNG has to be valid (see BaseRandom)
 */

void Simulation::run() {
    logMessage("starting... Number of walkers = %u",totalWalkers());
    time(&startTime);
    nLayers = _sample->nLayers();
    upperZBoundaries = _sample->zBoundaries();

    n = 0;

    ExponentialDistribution *stepLength = new ExponentialDistribution(this);
    CosThetaGenerator *deflCosine = new CosThetaGenerator(0,this); // I set g=0 without any particular reason
    IsotropicPsiGenerator *randomPsi = new IsotropicPsiGenerator(this);

    while(n < _totalWalkers) {
        currentTrajectory = new std::vector<MCfloat>();
        layer0 = 0;

        Walker *walker = source->constructWalker();
        walker->nInteractions.insert(walker->nInteractions.begin(),nLayers+2,0);

        layer0 = layerAt(walker->r0); //updates also onInterface flag

        memcpy(walker->k1,walker->k0,3*sizeof(MCfloat));
        onInterface = true; //treat newly generated walker as it were on an interface, i.e. do not scatter

        saveTrajectoryPoint(walker->r0);

#ifdef DEBUG_TRAJECTORY
        printf("%d\t",layer0);
        printf("%lf\t%lf\t%lf\t\t%lf", walker->r0[0], walker->r0[1], walker->r0[2],walker->k0[2]);
#endif

        MCfloat length;
        while(1) {
            //spin k1 (i.e. scatter) only if the material is scattering
            //and we're not on an interface, use the old k1 otherwise
            if(_sample->material(layer0)->ls > 0) {
                stepLength->setBeta(_sample->material(layer0)->ls);
                length = stepLength->spin();
                if(!onInterface) {
                    deflCosine->setg(_sample->material(layer0)->g);
                    MCfloat cosTheta = deflCosine->spin();
                    MCfloat sinTheta = sqrt(1-pow(cosTheta,2));
                    MCfloat psi = randomPsi->spin();
                    MCfloat cosPsi = cos(psi);
                    MCfloat sinPsi = sin(psi);

                    if(fabs(walker->k0[2]) > 0.999999) {
                        walker->k1[0] = sinTheta*cosPsi;
                        walker->k1[1] = sinTheta*sinPsi;
                        walker->k1[2] = cosTheta*sign<MCfloat>(walker->k0[2]);
                    }
                    else {
                        MCfloat temp = sqrt(1-pow(walker->k0[2],2));
                        walker->k1[0] = (sinTheta*(walker->k0[0]*walker->k0[2]*cosPsi - walker->k0[1]*sinPsi))/temp + cosTheta*walker->k0[0];
                        walker->k1[1] = (sinTheta*(walker->k0[1]*walker->k0[2]*cosPsi + walker->k0[0]*sinPsi))/temp + cosTheta*walker->k0[1];
                        walker->k1[2] = -sinTheta*cosPsi*temp + cosTheta*walker->k0[2];
                    }
                }
            }
            else { //no scattering
                length = std::numeric_limits<MCfloat>::infinity();
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

    time_t now;
    time(&now);

    logMessage("Completed in %.f seconds",difftime(now,startTime));

    printf("transmitted: %u\n",transmitted);
    printf("reflected: %u\n",reflected);
    printf("ballistic: %u\n",ballistic);
    printf("back-reflected: %u\n",backreflected);
}

/**
 * @brief Determines the layer index for the given position
 * @param r0
 * @return The index of the layer containing the point r0
 *
 * Interfaces are considered to belong to the preceding layer
 */

int Simulation::layerAt(MCfloat *r0) {
    onInterface = false;
    MCfloat z = r0[2];
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

void Simulation::move(Walker *walker, MCfloat length) {
    layer1 = layerAt(walker->r1);
    if(layer1 == layer0) {
        memcpy(walker->r0,walker->r1,3*sizeof(MCfloat));
        memcpy(walker->k0,walker->k1,3*sizeof(MCfloat));

        walker->walkTime += length/_sample->material(layer0)->v;
        walker->nInteractions[layer0]++; //not sure this is the right place to increment the interactions counter as well...
        return;
    }

    //handle interface

    MCfloat zBoundary=0;
    layer1 = layer0 + sign(walker->k1[2]);
    zBoundary = upperZBoundaries->at(min(layer0,layer1));

    MCfloat t = (zBoundary - walker->r0[2]) / walker->k1[2];
    MCfloat intersection[3];
    for (int i = 0; i < 3; ++i) {
        intersection[i] = walker->r0[i] + walker->k1[i]*t;
    }

    memcpy(walker->r0,intersection,3*sizeof(MCfloat)); //move to interface, r1 is now meaningless (should we set it to NULL?)
    onInterface = true;
    walker->walkTime += t/_sample->material(layer0)->v;

    //so we updated r0, now it's time to update k0

    MCfloat n0 = _sample->material(layer0)->n;
    MCfloat n1 = _sample->material(layer1)->n;

    if (n0 == n1) {
#ifdef DEBUG_TRAJECTORY
        printf("interface with same n...\n");
#endif
        layer0 = layer1;
        return;
    }

    //handle reflection and refraction

    MCfloat sinTheta1 = sqrt(1 - pow(walker->k1[2],2));
    MCfloat sinTheta2 = n0*sinTheta1/n1;

    if(sinTheta2 > 1) {
#ifdef DEBUG_TRAJECTORY
        printf("TIR ");
#endif
        reflect(walker);
    }
    else {
        MCfloat r;
        if(fresnelReflectionsEnabled)
        {
            //calculate the probability r(Theta1,n0,n1) of being reflected
            MCfloat cThetaSum, cThetaDiff; //cos(Theta1 + Theta2) and cos(Theta1 - Theta2)
            MCfloat sThetaSum, sThetaDiff; //sin(Theta1 + Theta2) and sin(Theta1 - Theta2)

            MCfloat cosTheta1 = fabs(walker->k1[2]);
            MCfloat cosTheta2 = sqrt(1 - pow(sinTheta2,2));

            cThetaSum = cosTheta1*cosTheta2 - sinTheta1*sinTheta2;
            cThetaDiff = cosTheta1*cosTheta2 + sinTheta1*sinTheta2;
            sThetaSum = sinTheta1*cosTheta2 + cosTheta1*sinTheta2;
            sThetaDiff = sinTheta1*cosTheta2 - cosTheta1*sinTheta2;
            r = 0.5*sThetaDiff*sThetaDiff*(cThetaDiff*cThetaDiff+cThetaSum*cThetaSum)/(sThetaSum*sThetaSum*cThetaDiff*cThetaDiff);
        }
        else
            r = 0;

        MCfloat xi = uniform_01<MCfloat>()(*mt);

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
    MCfloat n0 = _sample->material(layer0)->n; //I have already defined those quantities. should we pass them as arguments?
    MCfloat n1 = _sample->material(layer1)->n; //
    MCfloat sinTheta1 = sqrt(1 - pow(walker->k1[2],2)); //

    walker->k1[0] = n0*walker->k1[0]/n1;
    walker->k1[1] = n0*walker->k1[1]/n1;
    walker->k1[2] = sqrt(1 - pow(n0*sinTheta1/n1,2));
    layer0=layer1;
}

void Simulation::saveTrajectoryPoint(MCfloat *point) {
    if(!saveTrajectory)
        return;
    for (int i = 0; i < 3; ++i) {
        currentTrajectory->push_back(point[i]);
    }
}

void Simulation::reportProgress() const
{
    string msg;
    string s = str( format("Progress = %.1lf%% (%u / %u)") % (100.*currentWalker()/totalWalkers()) % currentWalker() % totalWalkers());
    logMessage(s);
}

BaseObject* Simulation::clone_impl() const
{
    Simulation *sim = new Simulation();
    sim->saveTrajectory = saveTrajectory;
    sim->fresnelReflectionsEnabled = fresnelReflectionsEnabled;
    sim->setSource((Source*)source->clone());
    sim->_sample = _sample;
    return sim;
}

void Simulation::setSaveTrajectoryEnabled(bool enabled) {
    saveTrajectory = enabled;
}

/**
 * @brief Returns the trajectory of the last simulated walker
 * @return
 */

const vector< vector <MCfloat>*> *Simulation::trajectories() const {
    return trajectoryPoints;
}
