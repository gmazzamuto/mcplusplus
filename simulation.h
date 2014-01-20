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
    int totalWalkers;
    int transmitted, reflected, ballistic, backreflected;
    Sample *sample;
    Source *source;
};

#endif // SIMULATION_H
