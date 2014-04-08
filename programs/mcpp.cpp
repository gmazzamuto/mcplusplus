#include <stdio.h>
#include "xmlparser.h"
#include <QApplication>
#include <QMainWindow>
#include "simulationviewer.h"

int main(int argc, char *argv[])
{
    XMLParser parser;
    Simulation *sim = parser.load(argv[1]);

    sim->setTotalWalkers(atoi(argv[2]));
    sim->setNThreads(atoi(argv[3]));

    sim->setSeed(0);
    sim->run();

    if(parser.showTrajectoryEnabled()) {
        QApplication a(argc, argv);
        QMainWindow w;
        SimulationViewer simViewer(sim);

        simViewer.setDisplayedOriginPos(0,0,-600);
        simViewer.setDisplayedAxisLength(500);

        w.setCentralWidget(&simViewer);
        w.show();

        a.exec();
    }

    return 0;
}

