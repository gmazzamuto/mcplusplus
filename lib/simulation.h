#ifndef SIMULATION_H
#define SIMULATION_H

#include "source.h"
#include "sample.h"

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
    void setTotalWalkers(int N);
    void setSample(Sample *sample);
    void setSource(Source *source);
    void setFresnelReflectionsEnabled(bool enable);
    void setNThreads(unsigned int value);
    unsigned long int totalWalkers() const;
    unsigned long int currentWalker() const;
    void setOutputFileName(const char *name);
    void run();
    void clear();
    const vector<vector<MCfloat> *> *trajectories() const;
    void reportProgress() const;

    enum walkerTypes {
        TRANSMITTED = 1 << 0,
        BALLISTIC = 1 << 1,
        REFLECTED = 1 << 2,
        BACKREFLECTED = 1 << 3,
    };

    void setWalkTimesSaveFlags(unsigned int value);
    void setExitPointsSaveFlags(unsigned int value);

private:
    const Sample *_sample;
    const Source *source;

    //flags
    bool saveTrajectory;
    bool fresnelReflectionsEnabled;

    //walker counters
    unsigned long int _totalWalkers;  /**< @brief total number of walkers to be simulated*/
    unsigned long int transmitted;  /**< @brief total number of walkers transmitted after at least one scattering event*/
    unsigned long int reflected;  /**< @brief total number of walkers reflected after at least one scattering event*/
    unsigned long int ballistic;  /**< @brief total number of walkers transmitted without undergoing any scattering event*/
    unsigned long int backreflected;  /**< @brief total number of walkers reflected without undergoing any scattering event*/
    unsigned long int n;

    unsigned int nThreads;

    //trajectories
    vector<vector <MCfloat>*> *trajectoryPoints;
    vector<MCfloat> *currentTrajectory;

    vector<MCfloat> transmittedExitPoints;
    vector<MCfloat> reflectedExitPoints;

    vector<MCfloat> transmittedWalkTimes, reflectedWalkTimes;

    //internal temporary variables
    const MCfloat *upperZBoundaries;
    const Material *materials;
    unsigned int nLayers;
    unsigned int layer0;
    unsigned int layer1;
    bool onInterface;
    time_t startTime;
    Walker *walker;

    MCfloat n0, n1, cosTheta1;

    const char *outputFile;

    unsigned int layerAt(const MCfloat *r0) const;
    void move(const MCfloat length);
    void reflect();
    void refract();
    void saveTrajectoryPoint(MCfloat *point);

    void runMultipleThreads();
    void runSingleThread();

    virtual BaseObject* clone_impl() const;

    void saveOutput(bool saveRNGState=false);
    unsigned int walkTimesSaveFlags, exitPointsSaveFlags;
};

#endif // SIMULATION_H
