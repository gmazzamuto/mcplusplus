#include "simulation.h"
#include "distributions.h"
#include "psigenerator.h"
#include <signal.h>

#include <boost/math/special_functions/sign.hpp>
#include <boost/thread.hpp>

#include "h5outputfile.h"

using namespace boost;
using namespace boost::math;
using namespace boost::math::constants;

Simulation *mostRecentInstance=NULL;
vector<boost::thread*> threads;
vector<Simulation *> sims;

MCfloat module(const MCfloat *x) {
    return sqrt(x[0]*x[0] + x[1]*x[1] + x[2]*x[2]);
}

MCfloat module2D(const MCfloat x, const MCfloat y) {
    return sqrt(x*x + y*y);
}

void sigUsr1Handler(int sig, siginfo_t *siginfo, void *context) {
    Simulation *sim = mostRecentInstance;

    if(sim == NULL)
        return;

    sim->reportProgress();
}

void installSigUSR1Handler() {
    struct sigaction sa;
    memset(&sa,0,sizeof(sa));
    sa.sa_sigaction = sigUsr1Handler;
    sa.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1,&sa,NULL);
}

void sigTermHandler(int sig, siginfo_t *siginfo, void *context) {
    fprintf(stderr, "SIGTERM received... closing all threads\n");
    for(uint i=0;i<sims.size();i++) {
        Simulation *sim = sims.at(i);
        if(sim == NULL)
            continue;
        sim->terminate();
    }
}

void installSigTermHandler() {
    struct sigaction sa;
    memset(&sa,0,sizeof(sa));
    sa.sa_sigaction = sigTermHandler;
    sa.sa_flags = SA_SIGINFO;
    sigaction(SIGTERM,&sa,NULL);
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
    trajectoryPoints = new vector<vector<MCfloat>*>();
    saveTrajectory = false;
    fresnelReflectionsEnabled = true;
    nThreads = 1;
    _totalWalkers = 0;
    outputFile = NULL;
    walkTimesSaveFlags = 0;
    exitPointsSaveFlags = 0;
    exitKVectorsDirsSaveFlags = 0;
    exitKVectorsSaveFlags = 0;
    timeOriginZ = 0;
    deflCosine.setParent(this);
    clear();
    mostRecentInstance = this;
    installSigUSR1Handler();
}

Simulation::~Simulation() {
    delete trajectoryPoints;
    if(mostRecentInstance == this)
        mostRecentInstance = NULL;
}

void Simulation::clear() {
    for (int i = 0; i < 4; ++i) {
        exitPoints[i].clear();
        exitKVectors[i].clear();
        walkTimes[i].clear();
        photonCounters[i] = 0;
    }

    _nInteractions = NULL;
    forceTermination = false;
}

void Simulation::setTotalWalkers(u_int64_t N) {
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

u_int64_t Simulation::totalWalkers() const
{
    return _totalWalkers;
}

u_int64_t Simulation::currentWalker() const
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
        if(!wasCloned()) {
            installSigTermHandler();
            sims.clear();
            sims.push_back(this);
            if(multipleRNGStates.size() == 0) {
                setSeed(0);
            }
            else
                setGeneratorState(multipleRNGStates[0]);
        }

        runSingleThread();

        if(!wasCloned())
            saveOutput();
    }
    else
        runMultipleThreads();

    stringstream stream;
    stream << "================\n";
    for (uint i = 0; i < 4; ++i) {
        stream << walkerIndexToString(i) << ": " << photonCounters[i] << endl;
    }

    cout << stream.str();

    time_t now;
    time(&now);

    logMessage("Completed in %.f seconds",difftime(now,startTime));
}

void Simulation::runMultipleThreads()
{
    threads.clear();
    sims.clear();
    installSigTermHandler();

    unsigned int walkersPerThread = totalWalkers()/nThreads;
    unsigned int remainder = totalWalkers() % nThreads;

    for (unsigned int n = 0; n < nThreads; ++n) {
        Simulation *sim = (Simulation *)clone();
        unsigned int nWalkers = walkersPerThread;
        if(n<remainder)
            nWalkers++;
        sim->setTotalWalkers(nWalkers);
        if(n<multipleRNGStates.size()) {
            sim->setSeed(n);
            sim->setGeneratorState(multipleRNGStates[n]);
        }
        else
            sim->setSeed(n);
        sims.push_back(sim);

        //launch thread
        threads.push_back(new boost::thread(workerFunc,sims.at(n)));
    }

    //wait for all threads to finish
    for (unsigned int n = 0; n < nThreads; ++n) {
        boost::thread * thread = threads.at(n);
        thread->join();

        Simulation *sim = sims.at(n);
        for (uint i = 0; i < 4; ++i) {
            photonCounters[i] += sim->photonCounters[i];
        }

        sim->saveOutput();
        delete sim;
        sims.at(n) = NULL;
    }
}

void Simulation::runSingleThread() {
    clear();
    logMessage("starting... Number of walkers = %Lu, original seed = %u",totalWalkers(), currentSeed());
    nLayers = _sample->nLayers();


    MCfloat *uzb = (MCfloat*)malloc((nLayers+2)*sizeof(MCfloat));
    Material *mat = (Material*)malloc((nLayers+2)*sizeof(Material));
    MCfloat *mus = (MCfloat*)malloc((nLayers+2)*sizeof(MCfloat));

    for (unsigned int i = 0; i < nLayers+1; ++i) {
        uzb[i]=_sample->zBoundaries()->at(i);
    }

    for (unsigned int i = 0; i < nLayers+2; ++i) {
        Material *m = _sample->material(i);
        m->setWavelegth(source->wavelength());
        mat[i]=*m;
        mus[i] = 1./mat[i].ls;
    }

    upperZBoundaries = uzb;
    materials = mat;
    this->mus = mus;

    n = 0;

    MCfloat timeOffset = 0;
    MCfloat pos[3];
    pos[0] = 0; pos[1] = 0;

    pos[2] = source->z0();
    layer0 = layerAt(pos);
    pos[2] = timeOriginZ;
    layer1 = layerAt(pos);

    uint initialLayer = layer0;
    uint leftLayer = min(layer0,layer1);
    uint rightLayer = max(layer0,layer1);
    MCfloat leftPoint = min(source->z0(), timeOriginZ);
    if(leftPoint == -1*numeric_limits<MCfloat>::infinity()) {
        leftPoint = min(upperZBoundaries[0],timeOriginZ);
        initialLayer = 0;
    }
    MCfloat rightPoint = max(source->z0(), timeOriginZ);
    if(leftLayer != rightLayer) { //add first and last portion of distance
        timeOffset += (upperZBoundaries[leftLayer] - leftPoint)/materials[leftLayer].v;
        timeOffset += (rightPoint - upperZBoundaries[rightLayer - 1])/materials[rightLayer].v;
        for (uint i = leftLayer + 1; i <= rightLayer - 1 ; ++i) {
            timeOffset += (upperZBoundaries[i] - upperZBoundaries[i-1])/materials[i].v;
        }
    }
    else {
        timeOffset += fabs(rightPoint - leftPoint)/materials[leftLayer].v;
    }
    timeOffset *= -1 * sign<MCfloat>(timeOriginZ - source->z0());

    layer0 = numeric_limits<unsigned int>::max(); //otherwise updateLayerVariables() won't work

    while(n < _totalWalkers && !forceTermination) {
        walkerExitedSample = false;
        vector<u_int64_t> nInteractions;
        _nInteractions = &nInteractions;
        if(saveTrajectory)
            currentTrajectory = new vector<MCfloat>();

        source->spin(&walker);
        if(walker.r0[2] == -1*numeric_limits<MCfloat>::infinity())
            walker.r0[2] = leftPoint;

        walker.walkTime += timeOffset;

        totalLengthInCurrentLayer = 0;
        updateLayerVariables(initialLayer);

        walker.swap_k0_k1();         //at first, the walker propagates
        kNeedsToBeScattered = false; //with the orignal k

        appendTrajectoryPoint(walker.r0);
        nInteractions.insert(nInteractions.begin(),nLayers+2,0);

#ifdef DEBUG_TRAJECTORY
        printf("%d\t",layer0);
        printf("%lf\t%lf\t%lf\t\t%lf", walker.r0[0], walker.r0[1], walker.r0[2],walker.k0[2]);
#endif

        MCfloat length;
        while(1) {
            //spin k1 (i.e. scatter) only if the material is scattering
            if(currentMaterial->ls != numeric_limits<MCfloat>::infinity()) {
                length = exponential_distribution<MCfloat>(currentMus)(*mt);
                if(kNeedsToBeScattered) {
                    nInteractions[layer0]++;

                    MCfloat cosTheta = deflCosine.spin();
                    MCfloat sinTheta = sqrt(1-pow(cosTheta,2));
                    MCfloat psi = uniform_01<MCfloat>()(*mt)*two_pi<MCfloat>(); //uniform in [0,2pi)
                    MCfloat cosPsi, sinPsi;
#ifdef DOUBLEPRECISION
                    sincos(psi,&sinPsi,&cosPsi);
#else
                    sincosf(psi,&sinPsi,&cosPsi);
#endif

                    if(fabs(walker.k0[2]) > 0.999999) {
                        walker.k1[0] = sinTheta*cosPsi;
                        walker.k1[1] = sinTheta*sinPsi;
                        walker.k1[2] = cosTheta*sign<MCfloat>(walker.k0[2]);
                    }
                    else {
                        MCfloat temp = sqrt(1-pow(walker.k0[2],2));
                        walker.k1[0] = (sinTheta*(walker.k0[0]*walker.k0[2]*cosPsi - walker.k0[1]*sinPsi))/temp + cosTheta*walker.k0[0];
                        walker.k1[1] = (sinTheta*(walker.k0[1]*walker.k0[2]*cosPsi + walker.k0[0]*sinPsi))/temp + cosTheta*walker.k0[1];
                        walker.k1[2] = -sinTheta*cosPsi*temp + cosTheta*walker.k0[2];
                    }

                    MCfloat mod = module(walker.k1);
                    walker.k1[0]/=mod;
                    walker.k1[1]/=mod;
                    walker.k1[2]/=mod;
                }
            }
            else { //no scattering
                length = numeric_limits<MCfloat>::infinity(); //move() will take the walker to the closest interface
            }

            //compute new position
            walker.r1[0] = walker.r0[0] + length*walker.k1[0];
            walker.r1[1] = walker.r0[1] + length*walker.k1[1];
            walker.r1[2] = walker.r0[2] + length*walker.k1[2];

#ifdef DEBUG_TRAJECTORY
            printf("\t%lf\n",walker.k1[2]);
#endif
            move(length);

#ifdef DEBUG_TRAJECTORY
            printf("%d\t",layer0);
            printf("%lf\t%lf\t%lf\t\t%lf", walker.r0[0], walker.r0[1], walker.r0[2],walker.k0[2]);
#endif
            appendTrajectoryPoint(walker.r0);

            if(walkerExitedSample)
                break;
        } //end of walker

        if(saveTrajectory)
            trajectoryPoints->push_back(currentTrajectory);

#ifdef DEBUG_TRAJECTORY
        printf("\nwalker reached layer %d\n",layer0);
#endif
        n++;
    }
    free(uzb);
    free(mat);
    free(mus);
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
    for (unsigned int i = 0; i < nLayers+1; ++i) {
        if(z <= sample()->zBoundaries()->at(i))
            return i;
    }
    return nLayers+1;
}

void Simulation::move(const MCfloat length) {
    if(walker.r1[2] > currLayerLowerBoundary && walker.r1[2] <= currLayerUpperBoundary)
    {
        walker.swap_r0_r1();
        walker.swap_k0_k1();

        totalLengthInCurrentLayer+=length;
        kNeedsToBeScattered = true;
        return;
    }

    //handle interface

    MCfloat zBoundary=0;
    layer1 = layer0 + sign(walker.k1[2]);
    zBoundary = upperZBoundaries[min(layer0,layer1)];

    MCfloat t = (zBoundary - walker.r0[2]) / walker.k1[2];
    for (int i = 0; i < 3; ++i) {
        walker.r0[i] = walker.r0[i] + walker.k1[i]*t; //move to intersection with interface, r1 is now meaningless
    }

    totalLengthInCurrentLayer+=t;
    kNeedsToBeScattered = false;

    //so we updated r0, now it's time to update k0

    n0 = materials[layer0].n;
    n1 = materials[layer1].n;

    if (n0 == n1) {
#ifdef DEBUG_TRAJECTORY
        printf("interface with same n...\n");
#endif
        switchToLayer(layer1);
    }
    else { //handle reflection and refraction

        MCfloat sinTheta0 = sqrt(1 - pow(walker.k1[2],2));
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

                MCfloat cosTheta0 = fabs(walker.k1[2]);

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

                if(xi <= r)
                    reflect();
                else
                    refract();
            }
            else
                refract();
        }
    }

    //check if walker exited the sample
    if(layer0 == nLayers + 1) {
        bool diffuselyTransmitted = false;
        for (unsigned int i = 1; i <= nLayers; ++i) {
            if((*_nInteractions)[i]) {

                appendWalker(TRANSMITTED);

                diffuselyTransmitted = true;
                break;
            }
        }
        if(!diffuselyTransmitted)
            appendWalker(BALLISTIC);
        walkerExitedSample = true;
    }

    if(layer0 == 0) {
        bool diffuselyReflected = false;
        for (unsigned int i = 1; i <= nLayers; ++i) {
            if((*_nInteractions)[i]) {

                appendWalker(REFLECTED);

                diffuselyReflected = true;
                break;
            }
        }
        if(!diffuselyReflected)
            appendWalker(BACKREFLECTED);
        walkerExitedSample = true;
    }
}

void Simulation::reflect() {
#ifdef DEBUG_TRAJECTORY
    printf("reflect ...\n");
#endif
    walker.k1[2] *= -1; //flip k1 along z
}

void Simulation::refract() {
#ifdef DEBUG_TRAJECTORY
    printf("refract ...\n");
#endif

    walker.k1[0] *= n0/n1;
    walker.k1[1] *= n0/n1;
    walker.k1[2] = sign<MCfloat>(walker.k1[2])*cosTheta1; //cosTheta1 is positive
    switchToLayer(layer1);
}

void Simulation::appendTrajectoryPoint(MCfloat *point) {
    if(!saveTrajectory)
        return;
    for (int i = 0; i < 3; ++i) {
        currentTrajectory->push_back(point[i]);
    }
}

void Simulation::appendExitPoint(enum walkerIndex idx)
{
    exitPoints[idx].push_back(walker.r0[0]);
    exitPoints[idx].push_back(walker.r0[1]);
}

void Simulation::appendExitKVector(walkerIndex idx)
{
    // we must use k1 instead of k0 because the walker can leave the sample only
    // when it is moving away from an interface
    if(exitKVectorsDirsSaveFlags & DIR_X)
        exitKVectors[idx].push_back(walker.k1[0]);
    if(exitKVectorsDirsSaveFlags & DIR_Y)
        exitKVectors[idx].push_back(walker.k1[1]);
    if(exitKVectorsDirsSaveFlags & DIR_Z)
        exitKVectors[idx].push_back(walker.k1[2]);
}

void Simulation::appendWalker(walkerIndex idx)
{
    photonCounters[idx]++;
    walkerFlags flags = walkerIndexToFlag(idx);

    if(exitPointsSaveFlags & flags)
        appendExitPoint(idx);

    if(walkTimesSaveFlags & flags)
        walkTimes[idx].push_back(walker.walkTime);

    if(exitKVectorsSaveFlags & flags)
        appendExitKVector(idx);
}

void Simulation::reportProgress() const
{
    string s = str( format("Progress = %.1lf%% (%u / %u) ") % (100.*currentWalker()/totalWalkers()) % currentWalker() % totalWalkers());
    stringstream ss;
    ss << s;
    if(currentWalker() > 0) {
        time_t now;
        time(&now);
        double secsPerWalker = difftime(now,startTime) / currentWalker();
        time_t eta = now + secsPerWalker*(totalWalkers()-currentWalker());
        struct tm * timeinfo;
        timeinfo = localtime (&eta);
        char buffer [80];
        strftime (buffer,80,"%F %T",timeinfo);
        ss << "ETA: " << buffer;
    }
    logMessage(ss.str());
}

void Simulation::setMultipleRNGStates(const vector<string> states)
{
    multipleRNGStates = states;
}

void Simulation::setTimeOriginZ(const MCfloat z)
{
    timeOriginZ = z;
}

BaseObject* Simulation::clone_impl() const
{
    Simulation *sim = new Simulation();
    sim->saveTrajectory = saveTrajectory;
    sim->fresnelReflectionsEnabled = fresnelReflectionsEnabled;
    sim->setSource((Source*)source->clone());
    sim->_sample = _sample;
    sim->outputFile = outputFile;
    sim->exitPointsSaveFlags = exitPointsSaveFlags;
    sim->walkTimesSaveFlags = walkTimesSaveFlags;
    sim->exitKVectorsSaveFlags = exitKVectorsSaveFlags;
    sim->exitKVectorsDirsSaveFlags = exitKVectorsDirsSaveFlags;
    sim->setTimeOriginZ(timeOriginZ);
    return sim;
}

void Simulation::saveOutput()
{
    H5OutputFile file;

    if(outputFile == NULL) {
        outputFile = "output.h5";
        logMessage("No output file name provided, writing to %s", outputFile);
        file.newFile(outputFile);
    }
    else if(!file.openFile(outputFile))
        return;

    file.saveRNGState(currentSeed(), generatorState());

    if(photonCounters[TRANSMITTED] && exitPointsSaveFlags & FLAG_TRANSMITTED)
        file.appendExitPoints(TRANSMITTED, exitPoints[TRANSMITTED].data(),exitPoints[TRANSMITTED].size());
    if(photonCounters[BALLISTIC] && exitPointsSaveFlags & FLAG_BALLISTIC)
        file.appendExitPoints(BALLISTIC, exitPoints[BALLISTIC].data(),exitPoints[BALLISTIC].size());
    if(photonCounters[REFLECTED] && exitPointsSaveFlags & FLAG_REFLECTED)
        file.appendExitPoints(REFLECTED, exitPoints[REFLECTED].data(),exitPoints[REFLECTED].size());
    if(photonCounters[BACKREFLECTED] && exitPointsSaveFlags & FLAG_BACKREFLECTED)
        file.appendExitPoints(BACKREFLECTED, exitPoints[BACKREFLECTED].data(),exitPoints[BACKREFLECTED].size());

    if(photonCounters[TRANSMITTED] && walkTimesSaveFlags & FLAG_TRANSMITTED)
        file.appendWalkTimes(TRANSMITTED, walkTimes[TRANSMITTED].data(),walkTimes[TRANSMITTED].size());
    if(photonCounters[BALLISTIC] && walkTimesSaveFlags & FLAG_BALLISTIC)
        file.appendWalkTimes(BALLISTIC, walkTimes[BALLISTIC].data(),walkTimes[BALLISTIC].size());
    if(photonCounters[REFLECTED] && walkTimesSaveFlags & FLAG_REFLECTED)
        file.appendWalkTimes(REFLECTED, walkTimes[REFLECTED].data(),walkTimes[REFLECTED].size());
    if(photonCounters[BACKREFLECTED] && walkTimesSaveFlags & FLAG_BACKREFLECTED)
        file.appendWalkTimes(BACKREFLECTED, walkTimes[BACKREFLECTED].data(),walkTimes[BACKREFLECTED].size());

    if(photonCounters[TRANSMITTED] && exitKVectorsSaveFlags & FLAG_TRANSMITTED)
        file.appendExitKVectors(TRANSMITTED, exitKVectors[TRANSMITTED].data(),exitKVectors[TRANSMITTED].size());
    if(photonCounters[BALLISTIC] && exitKVectorsSaveFlags & FLAG_BALLISTIC)
        file.appendExitKVectors(BALLISTIC, exitKVectors[BALLISTIC].data(),exitKVectors[BALLISTIC].size());
    if(photonCounters[REFLECTED] && exitKVectorsSaveFlags & FLAG_REFLECTED)
        file.appendExitKVectors(REFLECTED, exitKVectors[REFLECTED].data(),exitKVectors[REFLECTED].size());
    if(photonCounters[BACKREFLECTED] && exitKVectorsSaveFlags & FLAG_BACKREFLECTED)
        file.appendExitKVectors(BACKREFLECTED, exitKVectors[BACKREFLECTED].data(),exitKVectors[BACKREFLECTED].size());


    file.appendPhotonCounts(photonCounters);
    file.close();
    logMessage("Data written to %s", outputFile);
}

void Simulation::describe_impl() const
{
    logMessage("Sample description:");
    _sample->describe();
    logMessage("Source description:");
    source->describe();
}

void Simulation::switchToLayer(const uint layer)
{
    walker.walkTime += totalLengthInCurrentLayer/currentMaterial->v;
    totalLengthInCurrentLayer = 0;
    updateLayerVariables(layer);
}

void Simulation::updateLayerVariables(const uint layer) {
    layer0=layer;
    currentMaterial=&materials[layer0];
    deflCosine.setg(currentMaterial->g);
    currentMus = mus[layer0];
    if(layer0 == 0)
        currLayerLowerBoundary = -numeric_limits<MCfloat>::infinity();
    else
        currLayerLowerBoundary = upperZBoundaries[layer0-1];
    if(layer0 <= nLayers)
        currLayerUpperBoundary = upperZBoundaries[layer0];
    else
        currLayerUpperBoundary = numeric_limits<MCfloat>::infinity();
}

void Simulation::setRNG_impl()
{
    vector<string> states;
    states.push_back(generatorState());
    setMultipleRNGStates(states);
}

void Simulation::setExitPointsSaveFlags(unsigned int value)
{
    exitPointsSaveFlags = value;
}

void Simulation::setExitKVectorsSaveFlags(unsigned int value)
{
    exitKVectorsSaveFlags = value;
}

void Simulation::setExitKVectorsDirsSaveFlags(unsigned int value)
{
    exitKVectorsDirsSaveFlags = value;
}

void Simulation::terminate()
{
    forceTermination = true;
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
