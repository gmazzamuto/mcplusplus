#include "simulation.h"

Simulation::Simulation()
{
    sample = NULL;
    source = NULL;
    reset();
}

void Simulation::reset() {
    totalWalkers = 0;
    transmitted = 0;
    reflected = 0;
    ballistic = 0;
    backreflected = 0;
}

void Simulation::setTotalWalkers(int N) {
    totalWalkers = N;
}

void Simulation::setSample(Sample *sample) {
    this->sample = sample;
}

void Simulation::setSource(Source *source) {
    this->source = source;
}
