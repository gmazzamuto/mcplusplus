#ifndef SIMULATION_H
#define SIMULATION_H

#include "source.h"
#include "sample.h"
#include "costhetagenerator.h"
#include "histogram.h"

#define WALKER_BUFSIZE 1000

namespace MCPP {

/**
 * @brief The Simulation class is the core of the MC method
 *
 * In order to set up a simulation, you first have to specify a Sample and a
 * Source using setSample() and setSource(). Use setNWalkers() to specify the
 * total number of walkers to be simulated. The output file can be specified
 * using setOutputFileName(); output flags can be specified with their setter
 * functions: setWalkTimesSaveFlags(), etc.
 *
 * Before running the simulation, a RNG has to be initialized by either calling
 * setSeed(), loadGeneratorState() or setGeneratorState(). Use run() to start
 * the simulation.
 *
 * If you want to run the simulation in parallel threads, use setNThreads() to
 * specify the number of threads to be used. In this case multiple RNG states
 * can be loaded with setMultipleRNGStates(), otherwise sequential numbers from
 * 0 to the number of threads will be used as seeds for each thread.
 *
 * <h2>Signals</h2> The progress of the simulation currently running can be
 * printed on stderr by sending the USR1 signal to the process instantiating a
 * Simulation object. Progress of all threads can be printed by sending
 * SIGUSR2. Finally the TERM signal causes the simulation to terminate
 * gracefully (see terminate()).
 */

class Simulation : public BaseRandom
{
public:
    Simulation(BaseObject *parent=NULL);
    ~Simulation();

    const Sample *sample() const;
    void setSaveTrajectoryEnabled(bool enabled = true);
    void setNWalkers(u_int64_t N);
#ifdef SWIG
    %apply SWIGTYPE *DISOWN {Sample *sample};
#endif
    void setSample(Sample *sample);
#ifdef SWIG
    %apply SWIGTYPE *DISOWN {Source *source};
#endif
    void setSource(Source *source);
    void setFresnelReflectionsEnabled(bool enable);
    void setNThreads(unsigned int value);
    u_int64_t nWalkers() const;
    u_int64_t currentWalker() const;
    void setOutputFileName(const char *name);
    void run();
    void clear();
    const vector<vector<MCfloat> *> *trajectories() const;
    void reportProgress() const;
    void setMultipleRNGStates(const vector<string> states);
    void setTimeOriginZ(const MCfloat z);
    void setWalkTimesSaveFlags(unsigned int value);
    void setExitPointsSaveFlags(unsigned int value);
    void setExitKVectorsSaveFlags(unsigned int value);
    void setExitKVectorsDirsSaveFlags(unsigned int value);
    void terminate();
    uint nThreads();
#ifdef SWIG
    %apply SWIGTYPE *DISOWN {Histogram *hist};
#endif
    void addHistogram(Histogram *hist);
    void setRawOutputEnabled(bool enable);

private:
    const Sample *_sample;
    const Source *source;

    //flags
    bool saveTrajectory;
    bool fresnelReflectionsEnabled;
    MCfloat timeOriginZ;

    //walker counters
    u_int64_t _totalWalkers;  /**< @brief total number of walkers to be simulated*/
    u_int64_t photonCounters[4];
    u_int64_t n;
    uint nBuf;

    uint _nThreads;

    //trajectories
    vector<vector <MCfloat>*> *trajectoryPoints;
    vector<MCfloat> *currentTrajectory;

    vector<MCfloat> exitPoints[4];
    vector<MCfloat> walkTimes[4];
    vector<MCfloat> exitKVectors[4];

    //internal temporary variables
    CosThetaGenerator deflCosine;
    MCfloat currLayerLowerBoundary, currLayerUpperBoundary;
    const Material *currentMaterial;
    const MCfloat *mus;
    MCfloat currentMus;
    const MCfloat  *upperZBoundaries;
    const Material *materials;
    unsigned int nLayers;
    unsigned int layer0;
    unsigned int layer1;
    MCfloat *r0, *r1;
    MCfloat *k0, *k1;
    const vector<u_int64_t> *_nInteractions;
    MCfloat totalLengthInCurrentLayer;
    bool kNeedsToBeScattered;
    bool walkerExitedSample;
    time_t startTime;
    Walker walker;
    Walker walkerBuf[WALKER_BUFSIZE];

    MCfloat n0, n1, cosTheta1;

    const char *outputFile;

    unsigned int layerAt(const MCfloat *r0) const;
    void move(const MCfloat length);
    void reflect();
    void refract();

    void appendTrajectoryPoint(MCfloat *point);
    void appendWalkTime();
    void appendExitPoint(walkerType idx);
    void appendExitKVector(walkerType idx);
    void appendWalker(walkerType idx);

    void runMultipleThreads();
    bool runSingleThread();

    virtual BaseObject* clone_impl() const;
    virtual bool sanityCheck_impl() const;

    void saveRawOutput();

    inline void swap_r0_r1()
    {
        MCfloat *temp = r1;
        r1 = r0;
        r0 = temp;
    }

    inline void swap_k0_k1()
    {
        MCfloat *temp = k1;
        k1 = k0;
        k0 = temp;
    }

    unsigned int walkTimesSaveFlags, exitPointsSaveFlags, exitKVectorsDirsSaveFlags, exitKVectorsSaveFlags;
    vector<string> multipleRNGStates;
    vector<Histogram *> hists;
    bool forceTermination;
    bool rawOutputEnabled;

    virtual void describe_impl() const;
    void switchToLayer(const uint layer);
    virtual void setRNG_impl();
    void updateLayerVariables(const uint layer);
    void flushHistogram();
};

}
#endif // SIMULATION_H
