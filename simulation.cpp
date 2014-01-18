#include "simulation.h"

Simulation::Simulation()
{
}

Simulation::setTotalWalkers(int N) {
    totalWalkers = N;
}

void Simulation::setSample(Sample *sample) {
    this->sample = sample;
}

void Simulation::setSource(Source *source) {
    this->source = source;
}
