#include <stdio.h>
#include <MCPlusPlus/simulation.h>

using namespace MCPP;

int main(int argc, char *argv[])
{
    // define a sample consisting of two layers of different materials
    Sample *sample = new Sample(0);

    Vacuum vacuum;
    Material mat;

    mat.n = 1.5;
    mat.g = 0;
    mat.ls = 1000;

    sample->addLayer(&mat,1000); //add a layer of material mat and thickness 1000um

    Material mat2;
    mat2.n = 1.3;
    mat2.g = 0.5;
    mat2.ls = 700;

    sample->addLayer(&mat2,700);

    sample->setSurroundingEnvironment(&vacuum);

    // define a photon source
    Source *source = new PencilBeamSource(0);

    // define the main simulation object, simulate 1e9 photons using 8 parallel threads.
    // note that each thread will use increasing seeds starting from 0
    Simulation *sim = new Simulation(0);
    sim->setSample(sample);
    sim->setSource(source);
    sim->setNPhotons(1000000000);
    sim->setNThreads(8);
    sim->setSeed(0);
    sim->setOutputFileName("example.h5");



    // define and add several histograms to the simulation
    Histogram *hist;

    // 1) a histogram of the exit times
    hist = new Histogram(0);
    hist->setDataDomain(DATA_TIMES);
    hist->setPhotonTypeFlags(FLAG_TRANSMITTED);
    hist->setMax(1000);
    hist->setBinSize(2);
    hist->setName("times");
    hist->setSpatialVarianceEnabled(true);
    sim->addHistogram(hist);

    // 2) a histogram of the exit distances from the center
    hist = new Histogram(0);
    hist->setDataDomain(DATA_POINTS);
    hist->setPhotonTypeFlags(FLAG_TRANSMITTED);
    hist->setMax(1000000);
    hist->setBinSize(50);
    hist->setName("points");
    sim->addHistogram(hist);

    // 3) a bivariate histogram of the exit distances as a function of time
    hist = new Histogram(0);
    hist->setDataDomain(DATA_POINTS, DATA_TIMES);
    hist->setPhotonTypeFlags(FLAG_TRANSMITTED);
    hist->setMax(100000,1000);
    hist->setBinSize(500,2);
    hist->setName("points_vs_times");
    sim->addHistogram(hist);

    sim->run();

    return 0;
}
