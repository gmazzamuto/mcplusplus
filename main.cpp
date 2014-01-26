#include <stdio.h>
#include "simulation.h"
#include "psigenerator.h"

int main(int argc, char *argv[])
{
    Source *source = new Source();
    DeltaDistribution *deltaZ = new DeltaDistribution(50);
    PsiGenerator *unif2pi = new PsiGenerator;
    DeltaDistribution *delta0 = new DeltaDistribution(0);
    DeltaDistribution *delta1 = new DeltaDistribution(1);

    source->setk0Distribution(delta1,unif2pi);

    AbstractDistribution *rDistr[3];
    rDistr[0] = deltaZ;
    rDistr[1] = deltaZ;
    rDistr[2] = deltaZ;
    source->setr0Distribution(rDistr);

    source->setWalkTimeDistribution(delta0);

    Sample *sample = new Sample();

    Material *freeSpace = new FreeSpace();
    Material *material = new Material();

    material->ls = 1;
    material->g = 0;

    sample->addLayer(material,100);
    sample->setSurroundingEnvironment(freeSpace);

    Simulation *sim = new Simulation();

    sim->setSample(sample);
    sim->setSource(source);
    sim->setTotalWalkers(1);

    sim->setSeed(0);
    sim->run();

    return 0;
}
