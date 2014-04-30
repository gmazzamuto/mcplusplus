#include "h5outputfile.h"

#include <iostream>

const char outputFileName[] = "testOutput.h5";

void pass() {
    cout << "testBilayer PASSED" << endl;
    remove(outputFileName);
    exit(EXIT_SUCCESS);
}

void fail() {
    cout << "FAILED" << endl;
    remove(outputFileName);
    exit(EXIT_FAILURE);
}

int main() {
    stringstream ss;
    ss << "<MCPlusPlus>"
          "<materials>"
            "<material name=\"material1\" n=\"1.5\" ls=\"1\" g=\"0\"/>"
            "<material name=\"material2\" n=\"1.3\" ls=\"2\" g=\"0.5\"/>"
          "</materials>"
          "<MLSample left=\"Air\" right=\"Air\">"
          "  <layer material=\"material1\" thickness=\"40\" />"
          "  <layer material=\"material2\" thickness=\"40\" />"
          "</MLSample>"
          "<source type=\"PencilBeamSource\" walkTime=\"0\"/>"
          "<output walk-times=\"t\" exit-points=\"t\" exit-k=\"t\" exit-k-dirs=\"z\"/>"
          "<simulation time-origin-z=\"0\"/>"
          "</MCPlusPlus>";

    cerr << ss.str();

    H5OutputFile file;
    file.newFromXMLContent(ss.str(),outputFileName);
    file.close();

    cerr << "parsing...." << endl;
    XMLParser parser;
    parser.setXMLContent(ss.str());
    parser.parseAll();

    Simulation *sim = parser.simulation();
    sim->setTotalWalkers(100000);
    sim->setNThreads(4);
    sim->setOutputFileName(outputFileName);

    sim->run();

    delete sim;

    file.openFile(outputFileName);

    if(file.transmitted() != 5771) fail();
    if(file.ballistic() != 0) fail();
    if(file.reflected() != 90326) fail();
    if(file.backReflected() != 3903) fail();

    pass();
    return 0;
}

