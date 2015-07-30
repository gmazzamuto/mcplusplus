/*
  This file is part of MCPlusPlus.

  MCPlusPlus is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Mcplusplus is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with MCPlusPlus.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <MCPlusPlus/simulation.h>
#include <MCPlusPlus/distributions.h>
#include <MCPlusPlus/psigenerator.h>
#include <signal.h>

#include <boost/math/special_functions/sign.hpp>
#include <boost/math/constants/constants.hpp>
#include <boost/thread.hpp>

#include <MCPlusPlus/h5outputfile.h>

using namespace boost;
using namespace boost::math;
using namespace boost::math::constants;
using namespace MCPP;

Simulation *mainSimulation=NULL;
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

void sigUsr2Handler(int sig, siginfo_t *siginfo, void *context) {
    fprintf(stderr,"===============================\n");
    for (unsigned int n = 0; n < mainSimulation->nThreads(); ++n) {
        Simulation *sim = sims.at(n);
        if(sim == NULL)
            continue;
        sim->reportProgress();
    }
    fprintf(stderr,"===============================\n");
}

void installSigUSR1Handler() {
    struct sigaction sa;
    memset(&sa,0,sizeof(sa));
    sa.sa_sigaction = sigUsr1Handler;
    sa.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1,&sa,NULL);
}

void installSigUSR2Handler() {
    struct sigaction sa;
    memset(&sa,0,sizeof(sa));
    sa.sa_sigaction = sigUsr2Handler;
    sa.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR2,&sa,NULL);
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
    _nThreads = 1;
    _totalWalkers = 0;
    outputFile = NULL;
    walkTimesSaveFlags = 0;
    exitPointsSaveFlags = 0;
    exitKVectorsDirsSaveFlags = 0;
    exitKVectorsSaveFlags = 0;
    timeOriginZ = 0;
    deflCosine.setParent(this);
    setRawOutputEnabled(false);
    r0 = (MCfloat*)calloc(3,sizeof(MCfloat));
    r1 = (MCfloat*)calloc(3,sizeof(MCfloat));
    k0 = (MCfloat*)calloc(3,sizeof(MCfloat));
    k1 = (MCfloat*)calloc(3,sizeof(MCfloat));
    clear();
    mostRecentInstance = this;
    installSigUSR1Handler();
    addObjectToCheck((const BaseObject**)&_sample);
    addObjectToCheck((const BaseObject**)&source);
}

Simulation::~Simulation() {
    for (unsigned int i = 0; i < trajectoryPoints->size(); ++i) {
        delete trajectoryPoints->at(i);
    }
    delete trajectoryPoints;
    if(mostRecentInstance == this)
        mostRecentInstance = NULL;
}

/**
 * @brief Clears the simulation internal data
 */

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

/**
 * @brief Set the total number of walkers to simulated.
 * @param N
 */

void Simulation::setNPhotons(const u_int64_t N) {
    _totalWalkers = N;
}

void Simulation::setNWalkers(const u_int64_t N)
{
    setNPhotons(N);
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

/**
 * @brief Sets the number of parallel threads to be used
 * @param value
 */

void Simulation::setNThreads(unsigned int value)
{
    _nThreads = value;
}

/**
 * @brief The total number of photons to be simulated
 * @return
 */

u_int64_t Simulation::nPhotons() const
{
    return _totalWalkers;
}

u_int64_t Simulation::currentPhoton() const
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
 * Defaults to single thread operation. Use setNThreads() to set the number of
 * parallel threads used to run the simulation. \pre The RNG has to be valid
 * (see BaseRandom)
 */

void Simulation::run() {
    if(outputFile == NULL) {
        outputFile = "output.h5";
        logMessage("No output file name provided, using %s", outputFile);
    }
    installSigUSR2Handler();
    time(&startTime);
    for (size_t i = 0; i < hists.size(); ++i) {
        Histogram *h = hists[i];
        h->setScale(nPhotons());
        h->initialize();
    }

    if(_nThreads == 1) {
        if(!wasCloned()) {
            mainSimulation = this;
            installSigTermHandler();
            sims.clear();
            sims.push_back(this);
            if(multipleRNGStates.size() > 0)
                setGeneratorState(multipleRNGStates[0]);
        }

        if(!runSingleThread())
            return;

        if(!wasCloned())
            saveRawOutput();
    }
    else {
        mainSimulation = this;
        runMultipleThreads();
    }

    time_t now;
    time(&now);

    if(wasCloned()) {
        logMessage("Thread using seed %u completed in %.f seconds",currentSeed(),difftime(now,startTime));
        return;
    }

    stringstream stream;
    stream << "\n\n================\n";
    for (uint i = 0; i < 4; ++i) {
        stream << walkerTypeToString(i) << ": " << photonCounters[i] << endl;
    }

    logMessage("%s\nCompleted in %.f seconds\n================\n",stream.str().c_str(), difftime(now,startTime));

    for (size_t i = 0; i < hists.size(); ++i) {
        Histogram *h = hists[i];
        h->saveToFile(outputFile);
    }
}

void Simulation::runMultipleThreads()
{
    threads.clear();
    sims.clear();
    installSigTermHandler();

    u_int64_t walkersPerThread = nPhotons()/_nThreads;
    u_int64_t remainder = nPhotons() % _nThreads;

    for (unsigned int n = 0; n < _nThreads; ++n) {
        Simulation *sim = (Simulation *)clone();
        u_int64_t nWalkers = walkersPerThread;
        if(n<remainder)
            nWalkers++;
        sim->setNPhotons(nWalkers);
        sim->setSeed(currentSeed()+n);
        if(n<multipleRNGStates.size())
            sim->setGeneratorState(multipleRNGStates[n]);

        sims.push_back(sim);

        //launch thread
        threads.push_back(new boost::thread(workerFunc,sims.at(n)));
    }

    //wait for all threads to finish
    for (unsigned int n = 0; n < _nThreads; ++n) {
        boost::thread * thread = threads.at(n);
        thread->join();

        Simulation *sim = sims.at(n);

        for (uint i = 0; i < 4; ++i) {
            photonCounters[i] += sim->photonCounters[i];
        }

        for (size_t i = 0; i < hists.size(); ++i) {
            Histogram *h = hists[i];
            h->appendCounts((sim->hists[i]));
        }

        sim->saveRawOutput();

        if(mostRecentInstance == sim)
            mostRecentInstance = NULL;

        sims.at(n) = NULL;
        delete sim;
    }
}

bool Simulation::runSingleThread() {
    if(!sanityCheck())
        return false;

    clear();
    logMessage("starting... Number of walkers = %Lu, original seed = %u",nPhotons(), currentSeed());
    nLayers = _sample->nLayers();


    MCfloat *uzb = (MCfloat*)malloc((nLayers+2)*sizeof(MCfloat));
    Material *mat = (Material*)malloc((nLayers+2)*sizeof(Material));
    MCfloat *mus = (MCfloat*)malloc((nLayers+2)*sizeof(MCfloat));

    for (unsigned int i = 0; i < nLayers+1; ++i) {
        uzb[i]=_sample->zBoundaries()->at(i);
    }

    for (unsigned int i = 0; i < nLayers+2; ++i) {
        Material *m = _sample->material(i);
        m->setWavelength(source->wavelength());
        mat[i]=*m;
        mus[i] = 1./mat[i].ls;
    }

    upperZBoundaries = uzb;
    materials = mat;
    this->mus = mus;

    n = 0;
    nBuf = 0;

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
        if(nBuf == WALKER_BUFSIZE)
            flushHistogram();

        walkerExitedSample = false;
        vector<u_int64_t> nInteractions;
        _nInteractions = &nInteractions;
        if(saveTrajectory)
            currentTrajectory = new vector<MCfloat>();

        source->spin(&walker);
        memcpy(r0,walker.r0,3*sizeof(MCfloat));
        memcpy(k0,walker.k0,3*sizeof(MCfloat));
        if(r0[2] == -1*numeric_limits<MCfloat>::infinity())
            r0[2] = leftPoint;

        walker.walkTime += timeOffset;

        totalLengthInCurrentLayer = 0;
        updateLayerVariables(initialLayer);

        swap_k0_k1();         //at first, the walker propagates
        kNeedsToBeScattered = false; //with the orignal k

#ifdef ENABLE_TRAJECTORY
        if(saveTrajectory)
            appendTrajectoryPoint(r0);
#endif
        nInteractions.insert(nInteractions.begin(),nLayers+2,0);

#ifdef DEBUG_TRAJECTORY
        printf("%d\t",layer0);
        printf("%lf\t%lf\t%lf\t\t%lf", r0[0], r0[1], r0[2],k0[2]);
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
#endif
#ifdef LONGDOUBLEPRECISION
                    sincosl(psi,&sinPsi,&cosPsi);
#endif
#ifdef SINGLEPRECISION
                    sincosf(psi,&sinPsi,&cosPsi);
#endif

                    if(fabs(k0[2]) > 0.999999) {
                        k1[0] = sinTheta*cosPsi;
                        k1[1] = sinTheta*sinPsi;
                        k1[2] = cosTheta*sign<MCfloat>(k0[2]);
                    }
                    else {
                        MCfloat temp = sqrt(1-pow(k0[2],2));
                        k1[0] = (sinTheta*(k0[0]*k0[2]*cosPsi - k0[1]*sinPsi))/temp + cosTheta*k0[0];
                        k1[1] = (sinTheta*(k0[1]*k0[2]*cosPsi + k0[0]*sinPsi))/temp + cosTheta*k0[1];
                        k1[2] = -sinTheta*cosPsi*temp + cosTheta*k0[2];
                    }

                    MCfloat mod = sqrt(k1[0]*k1[0] + k1[1]*k1[1] + k1[2]*k1[2]);
                    k1[0]/=mod;
                    k1[1]/=mod;
                    k1[2]/=mod;
                }
            }
            else { //no scattering
                length = numeric_limits<MCfloat>::infinity(); //handleInterface() will take the walker to the closest interface
            }

            //compute new position
            r1[0] = r0[0] + length*k1[0];
            r1[1] = r0[1] + length*k1[1];
            r1[2] = r0[2] + length*k1[2];

#ifdef DEBUG_TRAJECTORY
            printf("\t%lf\n",k1[2]);
#endif

            if(r1[2] >= currLayerLowerBoundary && r1[2] <= currLayerUpperBoundary)
            {
                swap_r0_r1();
                swap_k0_k1();

                totalLengthInCurrentLayer+=length;
                kNeedsToBeScattered = true;
            }
            else
                handleInterface();


#ifdef DEBUG_TRAJECTORY
            printf("%d\t",layer0);
            printf("%lf\t%lf\t%lf\t\t%lf", r0[0], r0[1], r0[2],k0[2]);
#endif

#ifdef ENABLE_TRAJECTORY
            if(saveTrajectory)
                appendTrajectoryPoint(r0);
#endif

            if(walkerExitedSample)
                break;
        } //end of walker

#ifdef ENABLE_TRAJECTORY
        if(saveTrajectory)
            trajectoryPoints->push_back(currentTrajectory);
#endif

#ifdef DEBUG_TRAJECTORY
        printf("\nwalker reached layer %d\n",layer0);
#endif
        n++;
    }
    free(uzb);
    free(mat);
    free(mus);
    flushHistogram();
    return true;
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

/**
 * @brief Moves the walker according to r1 and k1.
 * @param length
 *
 * This function also takes care of handling the crossing of an interface
 */

void Simulation::handleInterface() {
    MCfloat zBoundary=0;
    layer1 = layer0 + sign(k1[2]);
    zBoundary = upperZBoundaries[min(layer0,layer1)];

    MCfloat t = (zBoundary - r0[2]) / k1[2];
    for (int i = 0; i < 3; ++i) {
        r0[i] = r0[i] + k1[i]*t; //move to intersection with interface, r1 is now meaningless
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

        MCfloat sinTheta0 = sqrt(1 - pow(k1[2],2));
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

                MCfloat cosTheta0 = fabs(k1[2]);

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
    k1[2] *= -1; //flip k1 along z
}

void Simulation::refract() {
#ifdef DEBUG_TRAJECTORY
    printf("refract ...\n");
#endif

    if(fabs(k1[2]) <= COSZERO)  //not normal incidence
    {
        k1[0] *= n0/n1;
        k1[1] *= n0/n1;
        k1[2] = sign<MCfloat>(k1[2])*cosTheta1; //cosTheta1 is positive
    }
    switchToLayer(layer1);
}

void Simulation::appendTrajectoryPoint(MCfloat *point) {
    for (int i = 0; i < 3; ++i) {
        currentTrajectory->push_back(point[i]);
    }
}

void Simulation::appendExitPoint(enum walkerType type)
{
    exitPoints[type].push_back(r0[0]);
    exitPoints[type].push_back(r0[1]);
}

void Simulation::appendExitKVector(walkerType type)
{
    // we must use k1 instead of k0 because the walker can leave the sample only
    // when it is moving away from an interface
    if(exitKVectorsDirsSaveFlags & DIR_X)
        exitKVectors[type].push_back(k1[0]);
    if(exitKVectorsDirsSaveFlags & DIR_Y)
        exitKVectors[type].push_back(k1[1]);
    if(exitKVectorsDirsSaveFlags & DIR_Z)
        exitKVectors[type].push_back(k1[2]);
}

void Simulation::appendWalker(walkerType type)
{
    photonCounters[type]++;
    Walker *w = &walkerBuf[nBuf++];

    memcpy(w->r0, r0, 3*sizeof(MCfloat));
    memcpy(w->k0, k1, 3*sizeof(MCfloat));
    w->walkTime = walker.walkTime;
    w->type = type;

    walkerFlags flags = walkerTypeToFlag(type);

    if(exitPointsSaveFlags & flags)
        appendExitPoint(type);

    if(walkTimesSaveFlags & flags)
        walkTimes[type].push_back(walker.walkTime);

    if(exitKVectorsSaveFlags & flags)
        appendExitKVector(type);
}

/**
 * @brief Print progress information
 *
 * This can also be triggered by sending the USR1 signal to the process
 * instantiating the Simulation object.
 */

void Simulation::reportProgress() const
{
    string s = str( format("Seed %u: progress = %.1lf%% (%u / %u) ") % currentSeed() % (100.*currentPhoton()/nPhotons()) % currentPhoton() % nPhotons());
    stringstream ss;
    ss << s;
    if(currentPhoton() > 0) {
        time_t now;
        time(&now);
        double secsPerWalker = difftime(now,startTime) / currentPhoton();
        time_t eta = now + secsPerWalker*(nPhotons()-currentPhoton());
        struct tm * timeinfo;
        timeinfo = localtime (&eta);
        char buffer [80];
        strftime (buffer,80,"%F %T",timeinfo);
        ss << "ETA: " << buffer;
    }
    logMessage(ss.str());
}

/**
 * @brief Sets a vector of RNG states to be used by the threads.
 * @param states
 *
 *
 * If the vector is not set or has fewer elements than the specified number of
 * threads to be run, a new RNG with a sequential seed will be created.
 */

void Simulation::setMultipleRNGStates(const vector<string> states)
{
    multipleRNGStates = states;
}

/**
 * @brief Sets the coordinate \f$ z_0 \f$ of the origin of times (\f$ t = 0 \f$)
 * @param z
 *
 *
 * If the photon source is placed at a coordinate \f$ z \neq z_0 \f$, then the
 * photon time distribution -- which is specified at the source position -- is
 * offset (with sign) by the time it takes for the photon to travel from the
 * source to \f$ z_0 \f$ (or viceversa) flying in a straight line and crossing
 * the boundaries at 90°. This property defaults to \f$ z = 0 \f$.
 */

void Simulation::setTimeOriginZ(const MCfloat z)
{
    timeOriginZ = z;
}

BaseObject* Simulation::clone_impl() const
{
    Simulation *sim = new Simulation(0);
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
    sim->setRawOutputEnabled(rawOutputEnabled);
    for (size_t i = 0; i < hists.size(); ++i) {
        Histogram *h = hists[i];
        sim->addHistogram((Histogram *)h->clone());
    }
    return sim;
}

bool Simulation::sanityCheck_impl() const
{
    for (size_t i = 0; i < hists.size(); ++i) {
        Histogram *h = hists[i];
        if(!h->sanityCheck())
            return false;
    }
    return true;
}

/**
 * @brief Save the simulated output to file
 *
 * The data is saved to the file specified by setOutputFileName(). If the
 * specified file does not exist, it is created. If the specified file exists,
 * data is appended to it. In all other cases "output.h5" is used.
 */

void Simulation::saveRawOutput()
{
    H5OutputFile file;

    if(access(outputFile,F_OK)<0) {
        file.newFile(outputFile,rawOutputEnabled);
    }
    else if(!file.openFile(outputFile)) {
        logMessage("Cannot open %s, writing to output.h5", outputFile);
        file.newFile("output.h5");
    }

    file.saveSample(_sample);
    file.appendPhotonCounts(photonCounters);

    if(!rawOutputEnabled)
        return;


    file.saveRNGState(currentSeed(), generatorState());

    for (uint type = 0; type < 4; ++type) {
        //exit points
        if(photonCounters[type] && exitPointsSaveFlags & walkerTypeToFlag(type))
            file.appendExitPoints((walkerType)type, exitPoints[type].data(),exitPoints[type].size());
        //walk times
        if(photonCounters[type] && walkTimesSaveFlags & walkerTypeToFlag(type))
            file.appendWalkTimes((walkerType)type, walkTimes[type].data(),walkTimes[type].size());
        //exit k vectors
        if(photonCounters[type] && exitKVectorsSaveFlags & walkerTypeToFlag(type))
            file.appendExitKVectors((walkerType)type, exitKVectors[type].data(),exitKVectors[type].size());
    }

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

/**
 * @brief Switches to the adjacent layer when the photon crosses an interface.
 * @param layer
 *
 *
 * This function updates the walker walk time with the total time spent in the previous
 * layer. It then calls updateLayerVariables().
 */

void Simulation::switchToLayer(const uint layer)
{
    walker.walkTime += totalLengthInCurrentLayer/currentMaterial->v;
    totalLengthInCurrentLayer = 0;
    updateLayerVariables(layer);
}

/**
 * @brief Updates the internal variables caching the layer parameters
 * @param layer
 *
 *
 * This function does nothing if layer equals layer0.
 */

void Simulation::updateLayerVariables(const uint layer) {
    if(layer0 == layer)
        return;
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

void Simulation::flushHistogram()
{
    for (size_t i = 0; i < hists.size(); ++i) {
        Histogram *h = hists[i];
        h->run(walkerBuf,nBuf);
    }
    nBuf = 0;
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

/**
 * @brief Gracefully terminates the currently running simulation
 *
 * Causes run() to return as soon as the photon currently being simulated is
 * completed. The simulation data is written to the output file as usual. This
 * function can also be triggered by sending the TERM signal to the process
 * instantiating the Simulation object; every thread will be terminated
 * gracefully.
 */

void Simulation::terminate()
{
    forceTermination = true;
}

uint Simulation::nThreads()
{
    return _nThreads;
}

/**
 * @brief Adds a histogram to be performed during the simulation
 * @param hist
 *
 * The simulation is automatically set as the histogram's parent. Histograms are
 * performed live during the simulation, At the end of the simulation, histograms
 * are saved in the H5 output file specified using setOutputFileName(), each in
 * a different dataset according to their names.
 */

void Simulation::addHistogram(Histogram *hist)
{
    hists.push_back(hist);
    hist->setParent(this);
}

/**
 * @brief Enables raw output
 * @param enable
 *
 * If enabled, per photon values are saved in the output file according to the
 * specified flags. See setWalkTimesSaveFlags(), setExitPointsSaveFlags(),
 * setExitKVectorsSaveFlags(). Keep in mind that this causes heavy memory usage
 * and big output file sizes.
 */

void Simulation::setRawOutputEnabled(bool enable)
{
    rawOutputEnabled = enable;
}


void Simulation::setWalkTimesSaveFlags(unsigned int value)
{
    walkTimesSaveFlags = value;
}

/**
 * @brief Enables saving trajectory points in memory.
 * @param enabled
 *
 *
 * This function works only if the library is compiled with the
 * ENABLE_TRAJECTORY option, which defaults to false for performance reasons.
 */

void Simulation::setSaveTrajectoryEnabled(bool enabled) {
    saveTrajectory = enabled;
}

/**
 * @brief Returns the trajectories of the simulated photons
 * @return
 */

const vector< vector <MCfloat>*> *Simulation::trajectories() const {
    return trajectoryPoints;
}
