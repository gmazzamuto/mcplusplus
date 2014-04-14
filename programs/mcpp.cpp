#include <stdio.h>
#include "xmlparser.h"
#include <QApplication>
#include <QMainWindow>
#include "h5outputfile.h"
#include "simulationviewer.h"

int main(int argc, char *argv[])
{
    XMLParser parser;
    parser.parseFile(argv[1]);

    Simulation *sim = parser.simulation();

    H5OutputFile file;
    file.newFile("mcpp-output.h5");
    file.writeXMLDescription(argv[1]);
    file.close();

    sim->setTotalWalkers(atoll(argv[2]));
    sim->setNThreads(atoi(argv[3]));

    sim->setSeed(0);
    sim->setOutputFileName("mcpp-output.h5");
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

