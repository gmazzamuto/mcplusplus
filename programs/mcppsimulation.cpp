#include <stdio.h>
#include "simulation.h"
#include "psigenerator.h"
#include <QApplication>
#include <QMainWindow>
#include "simulationviewer.h"

int main(int argc, char *argv[])
{
    Source *source = new Source();
    DeltaDistribution *deltaZ = new DeltaDistribution(50);
    IsotropicPsiGenerator *unif2pi = new IsotropicPsiGenerator;
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

    FreeSpace freeSpace;
    Material material;

    material.ls = 1;
    material.g = 0;
    material.n = 1;

    material.n = 1;
    sample->addLayer(material,50);
    material.n = 10;
    sample->addLayer(material,50);

    material.n = 1;
    sample->addLayer(material,50);
    material.n = 10;
    sample->addLayer(material,50);

    sample->setSurroundingEnvironment(freeSpace);

    QApplication a(argc, argv);

    Simulation *sim = new Simulation();

    sim->setSample(sample);
    sim->setSource(source);
    sim->setTotalWalkers(1);

    sim->setSeed(0);
    sim->setSaveTrajectoryEnabled(true);
    sim->run();

    QMainWindow w;
    SimulationViewer simViewer(sim);

    simViewer.setDisplayedOriginPos(0,0,-600);
    simViewer.setDisplayedAxisLength(500);

    w.setCentralWidget(&simViewer);
    w.show();

    a.exec();

    return 0;
}
