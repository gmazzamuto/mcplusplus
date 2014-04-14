#include "simulation.h"
#include "distributions.h"
#include "costhetagenerator.h"
#include "psigenerator.h"
#include <signal.h>

#include <boost/math/special_functions/sign.hpp>
#include <boost/thread.hpp>

#include "h5outputfile.h"

using namespace boost;
using namespace boost::math;
using namespace boost::math::constants;

Simulation *mostRecentInstance=NULL;

MCfloat module(const MCfloat *x) {
    return sqrt(x[0]*x[0] + x[1]*x[1] + x[2]*x[2]);
}

MCfloat module2D(const MCfloat x, const MCfloat y) {
    return sqrt(x*x + y*y);
}

void sigUsr1Handler(int signo) {
    Simulation *sim = mostRecentInstance;

    if(mostRecentInstance == NULL)
        return;

    sim->reportProgress();
}

void installSigHandler() {
    struct sigaction sa;
    sa.sa_handler = sigUsr1Handler;
    sigaction(SIGUSR1,&sa,NULL);
}

void workerFunc(Simulation *sim) {
    sim->run();
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
    nThreads = 1;
    _totalWalkers = 0;
    outputFile = NULL;
    walkTimesSaveFlags = 0;
    exitPointsSaveFlags = 0;
    clear();
    mostRecentInstance = this;
    installSigHandler();
}

Simulation::~Simulation() {
    delete trajectoryPoints;
    if(mostRecentInstance == this)
        mostRecentInstance = NULL;
}

void Simulation::clear() {
    transmitted = 0;
    reflected = 0;
    ballistic = 0;
    backreflected = 0;
    transmittedExitPoints.clear();
    reflectedExitPoints.clear();
    transmittedWalkTimes.clear();
    reflectedWalkTimes.clear();
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

void Simulation::setNThreads(unsigned int value)
{
    nThreads = value;
}

unsigned long Simulation::totalWalkers() const
{
    return _totalWalkers;
}

unsigned long int Simulation::currentWalker() const
{
    return n;
}

void Simulation::setOutputFileName(const char *name)
{
    outputFile = name;
}

/**
 * @brief Runs the simulation
 *
 * \pre The RNG has to be valid (see BaseRandom)
 */

void Simulation::run() {
    time(&startTime);

    if(nThreads == 1) {
        runSingleThread();
        if(!wasCloned())
            saveOutput(true);
    }
    else
        runMultipleThreads();

    stringstream stream;
    stream << "================\n";
    stream << "transmitted: " << transmitted << endl;
    stream << "reflected: " << reflected << endl;
    stream << "ballistic: " << ballistic << endl;
    stream << "back-reflected: " << backreflected << endl;

    cout << stream.str();

    time_t now;
    time(&now);

    logMessage("Completed in %.f seconds",difftime(now,startTime));
}

void Simulation::runMultipleThreads()
{
    vector<boost::thread*> threads;
    vector<Simulation *> sims;

    unsigned int walkersPerThread = totalWalkers()/nThreads;
    unsigned int remainder = totalWalkers() % nThreads;

    for (unsigned int n = 0; n < nThreads; ++n) {
        Simulation *sim = (Simulation *)clone();
        unsigned int nWalkers = walkersPerThread;
        if(n<remainder)
            nWalkers++;
        sim->setTotalWalkers(nWalkers);
        sim->setSeed(currentSeed()+n);
        sims.push_back(sim);
        threads.push_back(new boost::thread(workerFunc,sim));
    }

    for (unsigned int n = 0; n < nThreads; ++n) {
        boost::thread * thread = threads.at(n);
        thread->join();
    }

    for(unsigned int n = 0; n < nThreads; ++n) {
        Simulation *sim = sims.at(n);
        transmitted+=sim->transmitted;
        reflected+=sim->reflected;
        ballistic+=sim->ballistic;
        backreflected+=sim->backreflected;

        sim->saveOutput(n == 0 ? true : false);
        delete sim;
    }
}

void Simulation::runSingleThread() {
    clear();
    logMessage("starting... Number of walkers = %u, seed = %u",totalWalkers(), currentSeed());
    nLayers = _sample->nLayers();


    MCfloat *uzb = (MCfloat*)malloc((nLayers+2)*sizeof(MCfloat));
    Material *mat = (Material*)malloc((nLayers+2)*sizeof(Material));

    for (unsigned int i = 0; i < nLayers+1; ++i) {
        uzb[i]=_sample->zBoundaries()->at(i);
    }

    for (unsigned int i = 0; i < nLayers+2; ++i) {
        mat[i]=*(_sample->material(i));
    }

    upperZBoundaries = uzb;
    materials = mat;

    n = 0;

    CosThetaGenerator deflCosine(0,this); // I set g=0 without any particular reason

    while(n < _totalWalkers) {
        vector<unsigned long int> nInteractions;
        currentTrajectory = new std::vector<MCfloat>();
        layer0 = 0;

        walker = source->constructWalker();
        nInteractions.insert(nInteractions.begin(),nLayers+2,0);

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
            if(materials[layer0].ls > 0) {
                length = exponential_distribution<MCfloat>(materials[layer0].ls)(*mt);
                if(!onInterface) {

                    nInteractions[layer0]++;

                    deflCosine.setg(materials[layer0].g);
                    MCfloat cosTheta = deflCosine.spin();
                    MCfloat sinTheta = sqrt(1-pow(cosTheta,2));
                    MCfloat psi = uniform_01<MCfloat>()(*mt)*two_pi<MCfloat>(); //uniform in [0,2pi)
                    MCfloat cosPsi, sinPsi;
                    sincos(psi,&sinPsi,&cosPsi);

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
            move(length); //since I think that move() is conceptually the right place to increment walkTime it is convenient to pass the length as well
                                 //or we might let length to be a private Simulation member, and update its value accordingly with its remaining portion when a reflection takes place

#ifdef DEBUG_TRAJECTORY
            printf("%d\t",layer0);
            printf("%lf\t%lf\t%lf\t\t%lf", walker->r0[0], walker->r0[1], walker->r0[2],walker->k0[2]);
#endif
            saveTrajectoryPoint(walker->r0);

            if(layer0 == nLayers + 1) {
                bool diffuselyTransmitted = false;
                for (unsigned int i = 1; i <= nLayers; ++i) {
                    if(nInteractions[i]) {
                        transmitted++;

                        transmittedExitPoints.push_back(walker->r0[0]);
                        transmittedExitPoints.push_back(walker->r0[1]);

                        transmittedWalkTimes.push_back(walker->walkTime);

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
                for (unsigned int i = 1; i <= nLayers; ++i) {
                    if(nInteractions[i]) {
                        reflected++;

                        reflectedExitPoints.push_back(walker->r0[0]);
                        reflectedExitPoints.push_back(walker->r0[1]);

                        reflectedWalkTimes.push_back(walker->walkTime);

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
    free(uzb);
    free(mat);
}

/**
 * @brief Determines the layer index for the given position
 * @param r0
 * @return The index of the layer containing the point r0
 *
 * Interfaces are considered to belong to the preceding layer
 */

unsigned int Simulation::layerAt(const MCfloat *r0) const {
    MCfloat z = r0[2];
    if(z < upperZBoundaries[layer0]) { //search left
        unsigned int i = layer0 + 1;
        do {
            i--;
            if(r0[2] > upperZBoundaries[i])
                return i+1;
        }
        while(i!=0);
        return 0;
    }
    else //search right
    {
        for (unsigned int i = layer0; i < nLayers+1; ++i) {
            if(z < upperZBoundaries[i])
                return i;
        }
        return nLayers+1;
    }
}



void Simulation::move(const MCfloat length) {
    onInterface = false;
    layer1 = layerAt(walker->r1);
    if(layer1 == layer0) {
        memcpy(walker->r0,walker->r1,3*sizeof(MCfloat));
        memcpy(walker->k0,walker->k1,3*sizeof(MCfloat));

        walker->walkTime += length/materials[layer0].v;
        return;
    }

    //handle interface

    MCfloat zBoundary=0;
    layer1 = layer0 + sign(walker->k1[2]);
    zBoundary = upperZBoundaries[min(layer0,layer1)];

    MCfloat t = (zBoundary - walker->r0[2]) / walker->k1[2];
    MCfloat intersection[3];
    for (int i = 0; i < 3; ++i) {
        intersection[i] = walker->r0[i] + walker->k1[i]*t;
    }

    memcpy(walker->r0,intersection,3*sizeof(MCfloat)); //move to interface, r1 is now meaningless (should we set it to NULL?)
    onInterface = true;
    walker->walkTime += t/materials[layer0].v;

    //so we updated r0, now it's time to update k0

    n0 = materials[layer0].n;
    n1 = materials[layer1].n;

    if (n0 == n1) {
#ifdef DEBUG_TRAJECTORY
        printf("interface with same n...\n");
#endif
        layer0 = layer1;
        return;
    }

    //handle reflection and refraction

    MCfloat sinTheta0 = sqrt(1 - pow(walker->k1[2],2));
    MCfloat sinTheta1 = n0*sinTheta0/n1;

    if(sinTheta1 > 1) {
#ifdef DEBUG_TRAJECTORY
        printf("TIR ");
#endif
        reflect();
    }
    else {
#define COSZERO (1.0-1.0E-12)
        MCfloat r;
        cosTheta1 = sqrt(1 - pow(sinTheta1,2)); //will also be used by refract()
        if(fresnelReflectionsEnabled)
        {
            //calculate the probability r(Theta0,n0,n1) of being reflected
            MCfloat cThetaSum, cThetaDiff; //cos(Theta0 + Theta1) and cos(Theta0 - Theta1)
            MCfloat sThetaSum, sThetaDiff; //sin(Theta0 + Theta1) and sin(Theta0 - Theta1)

            MCfloat cosTheta0 = fabs(walker->k1[2]);

            if(cosTheta0 > COSZERO) { //normal incidence
                r = (n1-n0)/(n1+n0);
                r *= r;
            }
            else { //general case
                cThetaSum = cosTheta0*cosTheta1 - sinTheta0*sinTheta1;
                cThetaDiff = cosTheta0*cosTheta1 + sinTheta0*sinTheta1;
                sThetaSum = sinTheta0*cosTheta1 + cosTheta0*sinTheta1;
                sThetaDiff = sinTheta0*cosTheta1 - cosTheta0*sinTheta1;
                r = 0.5*sThetaDiff*sThetaDiff*(cThetaDiff*cThetaDiff+cThetaSum*cThetaSum)/(sThetaSum*sThetaSum*cThetaDiff*cThetaDiff);
            }

            MCfloat xi = uniform_01<MCfloat>()(*mt);

            if(xi <= r) {
                reflect(); //we come back to run() without keeping track of the extra unused step length
                return;
            }
        }
        refract();
    }
}

void Simulation::reflect() {
#ifdef DEBUG_TRAJECTORY
    printf("reflect ...\n");
#endif
    walker->k1[2] *= -1; //flip k1 along z
}

void Simulation::refract() {
#ifdef DEBUG_TRAJECTORY
    printf("refract ...\n");
#endif

    walker->k1[0] *= n0/n1;
    walker->k1[1] *= n0/n1;
    walker->k1[2] = sign<MCfloat>(walker->k1[2])*cosTheta1; //cosTheta1 is positive
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
    sim->outputFile = outputFile;
    return sim;
}

void Simulation::saveOutput(bool saveRNGState)
{
    H5OutputFile file;
    if(outputFile == NULL) {
        outputFile = "output.h5";
        logMessage("No output file name provided, writing to %s", outputFile);
        file.newFile(outputFile);
    }
    else if(!file.openFile(outputFile))
        return;

    if(saveRNGState)
        file.saveRNGState(generatorState());
    if(transmitted && exitPointsSaveFlags & TRANSMITTED)
        file.appendTransmittedExitPoints(transmittedExitPoints.data(),2*transmitted);
    if(reflected && exitPointsSaveFlags & REFLECTED)
        file.appendReflectedExitPoints(reflectedExitPoints.data(),2*reflected);
    if(transmitted && walkTimesSaveFlags & TRANSMITTED)
        file.appendTransmittedWalkTimes(transmittedWalkTimes.data(),transmitted);
    if(reflected && walkTimesSaveFlags & REFLECTED)
        file.appendReflectedWalkTimes(reflectedWalkTimes.data(),reflected);
    file.close();
    logMessage("Data written to %s", outputFile);
}

void Simulation::setExitPointsSaveFlags(unsigned int value)
{
    exitPointsSaveFlags = value;
}


void Simulation::setWalkTimesSaveFlags(unsigned int value)
{
    walkTimesSaveFlags = value;
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
