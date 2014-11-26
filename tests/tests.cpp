#include "tests.h"

void testBilayer() {
    Material mat1, mat2;

    mat1.n=1.5;
    mat1.ls = 1;
    mat1.g = 0;

    mat2.n=1.3;
    mat2.ls = 2;
    mat2.g = 0.5;

    Sample *sample = new Sample();
    sample->addLayer(&mat1,40);
    sample->addLayer(&mat2,40);
    sample->setSurroundingEnvironment(new Air());

    Source *src = new PencilBeamSource();
    src->setWalkTimeDistribution(new DeltaDistribution(0));

    Simulation *sim = new Simulation();
    sim->setSample(sample);
    sim->setSource(src);
    sim->setOutputFileName(outputFileName);

    sim->setNWalkers(100000);
    sim->setNThreads(4);
    sim->setSeed(0);

    sim->run();

    delete sim;
}
