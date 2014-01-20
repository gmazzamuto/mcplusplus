#ifndef SIMULATION_H
#define SIMULATION_H

#include "source.h"
#include "sample.h"

class Simulation
{
public:
    Simulation();
    void setTotalWalkers(int N);
    void setSample(Sample *sample);
    void setSource(Source *source);
    void run();
    void reset();

protected:
    bool isRunning;
    int totalWalkers;  /**< @brief total number of walkers to be simulated*/
    int transmitted;  /**< @brief total number of walkers transmitted after at least one scattering event*/
    int reflected;  /**< @brief total number of walkers reflected after at least one scattering event*/
    int ballistic;  /**< @brief total number of walkers transmitted without undergoing any scattering event*/
    int backreflected;  /**< @brief total number of walkers reflected without undergoing any scattering event*/
    Sample *sample;
    Source *source;
};

#endif // SIMULATION_H
