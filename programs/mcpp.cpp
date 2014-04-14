#include <stdio.h>
#include <QApplication>
#include <QMainWindow>
#include "h5outputfile.h"
#include "simulationviewer.h"
#include "xmlparser.h"

int main(int argc, char *argv[])
{
    Simulation *sim;

    H5File h5File;
    H5OutputFile file;
    XMLParser *parser;

    bool isValidH5 = false;

    try {
        isValidH5 = h5File.isHdf5("mcpp-output.h5");
    }
    catch (Exception e) {}

    if(!isValidH5)
    {
        parser = new XMLParser();
        parser->parseFile(argv[1]);
        file.newFile("mcpp-output.h5");
        file.writeXMLDescription(argv[1]);
        sim = parser->simulation();
        sim->setSeed(0);
        file.close();
    }
    else {
        file.openFile("mcpp-output.h5");
        sim = file.simulation();
        parser = file.xmlParser();
    }

    sim->setTotalWalkers(atoll(argv[2]));
    sim->setNThreads(atoi(argv[3]));

    sim->setOutputFileName("mcpp-output.h5");
    sim->run();

    if(parser->showTrajectoryEnabled())
    {
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

