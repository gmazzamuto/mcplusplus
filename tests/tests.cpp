#include "tests.h"

using namespace MCPP;

void testBilayer(const char *outputFileName) {
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

    Simulation *sim = new Simulation(0);
    sim->setSample(sample);
    sim->setSource(src);
    sim->setOutputFileName(outputFileName);


    Histogram *hist = new Histogram(0);
    hist->setDataDomain(DATA_TIMES);
    hist->setPhotonTypeFlags(FLAG_TRANSMITTED);
    hist->setMax(50);
    hist->setBinSize(1);
    hist->setName("times");
    double exponents[1] = {2};
    hist->enableSpatialMoments(exponents,1);
    sim->addHistogram(hist);

    hist = new Histogram(0);
    hist->setDataDomain(DATA_POINTS);
    hist->setPhotonTypeFlags(FLAG_TRANSMITTED);
    hist->setMax(500);
    hist->setBinSize(2);
    hist->setName("points");
    sim->addHistogram(hist);

    hist = new Histogram(0);
    hist->setDataDomain(DATA_K);
    hist->setPhotonTypeFlags(FLAG_TRANSMITTED);
    hist->setMax(90);
    hist->setBinSize(5);
    hist->setName("k");
    sim->addHistogram(hist);

    hist = new Histogram(0);
    hist->setDataDomain(DATA_POINTS,DATA_TIMES);
    hist->setPhotonTypeFlags(FLAG_TRANSMITTED);
    hist->setMax(500,50);
    hist->setBinSize(2,1);
    hist->setName("points_vs_time");
    sim->addHistogram(hist);


    sim->setNPhotons(1000000);
    sim->setNThreads(4);
    sim->setSeed(0);

    sim->run();

    delete sim;
}
