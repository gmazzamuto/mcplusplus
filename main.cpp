#include <stdio.h>
#include "simulation.h"

int main(int argc, char *argv[])
{
    Simulation *sim = new Simulation();
    Source *source = new PencilBeamSource();
    Sample *sample = new Sample();

    Material *freeSpace = new FreeSpace();
    Material *material = new Material();

    material->ls = 1;
    material->g = 0;

    sample->addLayer(material,100);
    sample->setSurroundingEnvironment(freeSpace);

    sim->setSample(sample);
    sim->setSource(source);

    sim->run();

    return 0;
}
