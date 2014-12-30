#include <stdio.h>
#include "simulation.h"
#include "psigenerator.h"
#include <QApplication>
#include <QMainWindow>
#include "simulationviewer.h"

using namespace MCPP;

int main(int argc, char *argv[])
{
    Source *source = new Source();
    DeltaDistribution *deltaZ = new DeltaDistribution(20);
    IsotropicPsiGenerator *unif2pi = new IsotropicPsiGenerator;
    DeltaDistribution *delta0 = new DeltaDistribution(0);
    DeltaDistribution *delta1 = new DeltaDistribution(1);

    source->setk0Distribution(delta1,unif2pi);

    AbstractDistribution *rDistr[2];
    rDistr[0] = deltaZ;
    rDistr[1] = deltaZ;
    source->setr0Distribution(rDistr[0],rDistr[1],20);

    source->setWalkTimeDistribution(delta0);

    Sample *sample = new Sample();

    Air freeSpace;
    Material material;

    material.ls = 0.5;
    material.g = 0;
    material.n = 1.5;

    sample->addLayer(&material,40);
    sample->addLayer(&freeSpace,40);
    sample->addLayer(&material,40);

    sample->setSurroundingEnvironment(&freeSpace);

    Simulation *sim = new Simulation();

    sim->setSample(sample);
    sim->setSource(source);
    sim->setNWalkers(atoi(argv[1]));
    sim->setNThreads(atoi(argv[2]));

    sim->setSeed(0);
    sim->setSaveTrajectoryEnabled(true);
    sim->run();

    QApplication a(argc, argv);
    QMainWindow w;
    SimulationViewer simViewer(sim);

    simViewer.setDisplayedOriginPos(0,0,-600);
    simViewer.setDisplayedAxisLength(500);

    w.setCentralWidget(&simViewer);
    w.show();

    a.exec();


    return 0;
}
