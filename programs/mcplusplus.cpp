#include <stdio.h>
#include <QApplication>
#include <QMainWindow>
#include <fstream>
#include "h5outputfile.h"
#include "simulationviewer.h"
#include "xmlparser.h"

char *progName;

void usage(FILE *f) {
    fprintf(f, "\n"
            "usage: %s [options] input_file"
            "\n"
            "\n"
            "where input_file is an XML file or a H5 file\n"
            "\n"
            "[options]:\n"
            "\t -h display help\n"
            "\t -w number of walkers (long long int, defaults to 1000000000)\n"
            "\t -t number of threads (defaults to 1)\n"
            "\n", progName);
}

int main(int argc, char *argv[])
{
    uint nThreads = 1;
    u_int64_t nWalkers = 1000000000;
    progName = argv[0];

    extern char *optarg;
    extern int optind;

    char *inputFileName = NULL;

    int c;

    while ((c = getopt(argc, argv, "hw:t:")) != -1)
        switch (c) {
        case 'h':
            usage(stdout);
            exit(EXIT_SUCCESS);
            break;

        case 'w':
            nWalkers=atoll(optarg);
            break;

        case 't':
            nThreads=atoi(optarg);
            break;

        default:
            break;
        }

    if(argc - optind < 1) {
        fprintf(stderr,"Error: input file not specified\n");
        usage(stderr);
        exit(EXIT_FAILURE);
    }

    inputFileName = argv[optind++];

    Simulation *sim;

    H5File h5File;
    H5OutputFile file;
    file.setXMLParserEnabled(true);
    XMLParser *parser;
    string outputFileName;
    std::string xmlCmdLine;

    if(access(inputFileName,R_OK)<0) {
        cerr << "ERROR: cannot access file " << inputFileName << endl;
        exit(EXIT_FAILURE);
    }

    bool isValidH5 = false;

    try {
        isValidH5 = h5File.isHdf5(inputFileName);
    }
    catch (Exception e) {}

    if(!isValidH5) //input file is XML
    {
        //read whole xml file to string
        std::ifstream f(inputFileName);
        xmlCmdLine = std::string((std::istreambuf_iterator<char>(f)),
                                 std::istreambuf_iterator<char>());

        parser = new XMLParser();
        try {
            parser->parseFile(inputFileName);
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
            file.newFromXML(inputFileName,outputFileName.c_str());
            sim = parser->simulation();
            sim->setSeed(0);
            file.close();
        }
        else
            cout << "Appending to " << outputFileName << endl;
    }
    else { //input file is H5
        outputFileName = inputFileName;
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

    sim->setNWalkers(nWalkers);
    sim->setNThreads(nThreads);

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

