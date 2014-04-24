#ifndef SIMULATION_H
#define SIMULATION_H

#include "source.h"
#include "sample.h"
#include "costhetagenerator.h"

/**
 * @brief Base simulation class.
 *
 * \note As the simulation runs the class updates counters for transmitted,
 * reflected and ballistically transmitted walkers. A counter for backreflected
 * walkers (i.e. walkers reflected because of refractive index mismatch before
 * they can even enter the actual sample or first layer) is not included,
 * because backreflection has not been contemplated in any circumstance.
 * The reason is twofold: ignoring backreflection allows to directly compare
 * counters from simulations whose sources are either inside the sample or
 * placed on its entrance surface. Secondly, in many real case applications a
 * beam source is \em focused on the entrance surface of a given sample: if such
 * sample is enclosed between - let's say - glass slides, backreflection would
 * have occurred \em before the walker is constructed, with practically unknown
 * incidence angles.
 */

class Simulation : public BaseRandom
{
public:
    Simulation(BaseObject *parent=NULL);
    ~Simulation();

    const Sample *sample() const;
    void setSaveTrajectoryEnabled(bool enabled = true);
    void setTotalWalkers(u_int64_t N);
    void setSample(Sample *sample);
    void setSource(Source *source);
    void setFresnelReflectionsEnabled(bool enable);
    void setNThreads(unsigned int value);
    u_int64_t totalWalkers() const;
    u_int64_t currentWalker() const;
    void setOutputFileName(const char *name);
    void run();
    void clear();
    const vector<vector<MCfloat> *> *trajectories() const;
    void reportProgress() const;
    void setMultipleRNGStates(const vector<string> states);
    void setTimeOriginZ(const MCfloat z);

    enum walkerSaveFlags {
        SAVE_TRANSMITTED = 1 << 0,
        SAVE_BALLISTIC = 1 << 1,
        SAVE_REFLECTED = 1 << 2,
        SAVE_BACKREFLECTED = 1 << 3,
        SAVE_ALL_WALKERS = 0xf,
    };

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
    u_int64_t transmitted;  /**< @brief total number of walkers transmitted after at least one scattering event*/
    u_int64_t reflected;  /**< @brief total number of walkers reflected after at least one scattering event*/
    u_int64_t ballistic;  /**< @brief total number of walkers transmitted without undergoing any scattering event*/
    u_int64_t backreflected;  /**< @brief total number of walkers reflected without undergoing any scattering event*/
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
    MCfloat totalLengthInCurrentLayer;
    bool kNeedsToBeScattered;
    time_t startTime;
    Walker *walker;

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

    void runMultipleThreads();
    void runSingleThread();

    virtual BaseObject* clone_impl() const;

    void saveOutput();

    unsigned int walkTimesSaveFlags, exitPointsSaveFlags, exitKVectorsDirsSaveFlags, exitKVectorsSaveFlags;
    vector<string> multipleRNGStates;
    bool forceTermination;

    virtual void describe_impl() const;
    void switchToLayer(const uint layer);
};

#endif // SIMULATION_H
