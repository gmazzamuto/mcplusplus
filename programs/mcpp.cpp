#include <stdio.h>
#include <QApplication>
#include <QMainWindow>
#include <fstream>
#include "h5outputfile.h"
#include "simulationviewer.h"
#include "xmlparser.h"

int main(int argc, char *argv[])
{
    Simulation *sim;

    H5File h5File;
    H5OutputFile file;
    file.setXMLParserEnabled(true);
    XMLParser *parser;
    string outputFileName;
    std::string xmlCmdLine;

    if(access(argv[1],R_OK)<0) {
        cerr << "ERROR: cannot access file " << argv[1] << endl;
        exit(EXIT_FAILURE);
    }

    bool isValidH5 = false;

    try {
        isValidH5 = h5File.isHdf5(argv[1]);
    }
    catch (Exception e) {}

    if(!isValidH5) //input file is XML
    {
        //read whole xml file to string
        std::ifstream f(argv[1]);
        xmlCmdLine = std::string((std::istreambuf_iterator<char>(f)),
                                 std::istreambuf_iterator<char>());

        parser = new XMLParser();
        try {
            parser->parseFile(argv[1]);
        } catch (XMLParser::Exception e) {
            cerr << "Error: " << e.str << endl;
            exit(EXIT_FAILURE);
        }
        outputFileName = parser->outputFileName();
        if(outputFileName == "") {
            outputFileName = "mcpp-output.h5";
            cerr << "WARNING: No output file name specified, saving data to " << outputFileName << endl;
        }
        if(access(outputFileName.c_str(),R_OK)<0) {//the h5 file does not exist
            cout << "Creating new file " << outputFileName << endl;
            file.newFromXML(argv[1],outputFileName.c_str());
            sim = parser->simulation();
            sim->setSeed(0);
            file.close();
        }
        else
            cout << "Appending to " << outputFileName << endl;
    }
    else { //input file is H5
        outputFileName = argv[1];
        cout << "Appending to " << outputFileName << endl;
    }

    if(!file.openFile(outputFileName.c_str())) {
        cerr << "ERROR: Cannot open " << outputFileName << endl;
        exit(EXIT_FAILURE);
    }
    string xmlOnDisk = file.readXMLDescription();
    if(xmlCmdLine != "") {
        if(xmlOnDisk.compare(xmlCmdLine) != 0) {
            cerr << endl;
            cerr << "ERROR: The provided xml file does not match with the one found in " << outputFileName << ":"<< endl << endl;
            cerr << xmlOnDisk << endl;
            exit(EXIT_FAILURE);
        }
    }

    sim = file.simulation();
    parser = file.xmlParser();

    file.close();

    sim->setNWalkers(atoll(argv[2]));
    sim->setNThreads(atoi(argv[3]));

    sim->setOutputFileName(outputFileName.c_str());
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

