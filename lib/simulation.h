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
    void setSnellReflectionsEnabled(bool enable);
    int totalWalkers();
    int currentWalker();
    void run();
    void reset();
    const vector<vector<MCfloat> *> *trajectories() const;

private:
    int _totalWalkers;  /**< @brief total number of walkers to be simulated*/
    int transmitted;  /**< @brief total number of walkers transmitted after at least one scattering event*/
    int reflected;  /**< @brief total number of walkers reflected after at least one scattering event*/
    int ballistic;  /**< @brief total number of walkers transmitted without undergoing any scattering event*/
    int backreflected;  /**< @brief total number of walkers reflected without undergoing any scattering event*/
    int n;

    const Sample *_sample;
    const Source *source;
    const deque<MCfloat> *upperZBoundaries;
    int nLayers;
    int layer0;
    int layer1;
    bool onInterface;
    bool saveTrajectory;
    bool snellReflectionsEnabled;

    int layerAt(MCfloat *r0);
    void move(Walker *walker, MCfloat length);
    void reflect(Walker *walker);
    void refract(Walker *walker);
    void saveTrajectoryPoint(MCfloat *point);
    vector<vector <MCfloat>*> *trajectoryPoints;
    vector<MCfloat> *currentTrajectory;
};

#endif // SIMULATION_H
