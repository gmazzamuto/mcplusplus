#ifndef SIMULATION_H
#define SIMULATION_H

#include "source.h"
#include "sample.h"
#include "costhetagenerator.h"

/**
 * @brief The Simulation class is the core of the MC method
 */

class Simulation : public BaseRandom
{
public:
    Simulation(BaseObject *parent=NULL);
    ~Simulation();

    const Sample *sample() const;
    void setSaveTrajectoryEnabled(bool enabled = true);
    void setNWalkers(u_int64_t N);
    void setSample(Sample *sample);
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

    uint nThreads;

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
    const vector<u_int64_t> *_nInteractions;
    MCfloat totalLengthInCurrentLayer;
    bool kNeedsToBeScattered;
    bool walkerExitedSample;
    time_t startTime;
    Walker walker;

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
    void runSingleThread();

    virtual BaseObject* clone_impl() const;

    void saveOutput();

    unsigned int walkTimesSaveFlags, exitPointsSaveFlags, exitKVectorsDirsSaveFlags, exitKVectorsSaveFlags;
    vector<string> multipleRNGStates;
    bool forceTermination;

    virtual void describe_impl() const;
    void switchToLayer(const uint layer);
    virtual void setRNG_impl();
    void updateLayerVariables(const uint layer);
};

#endif // SIMULATION_H
