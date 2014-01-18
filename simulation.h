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

protected:
    bool isRunning;
    int totalWalkers;
    int transmitted, reflected, ballistic, backreflected;
};

#endif // SIMULATION_H