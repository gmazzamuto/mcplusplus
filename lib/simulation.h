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
    void setTotalWalkers(int N);
    void setSample(Sample *sample);
    void setSource(Source *source);
    void run();
    void reset();

private:
    int totalWalkers;  /**< @brief total number of walkers to be simulated*/
    int transmitted;  /**< @brief total number of walkers transmitted after at least one scattering event*/
    int reflected;  /**< @brief total number of walkers reflected after at least one scattering event*/
    int ballistic;  /**< @brief total number of walkers transmitted without undergoing any scattering event*/
    Sample *sample;
    Source *source;

    vector<double> *upperZBoundaries;
    int nLayers;
    int currentLayer;

    void updateCurrentLayer(double *r0);
    int layerAt(double *r0) const;
};

#endif // SIMULATION_H
