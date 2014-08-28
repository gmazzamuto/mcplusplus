#include "tests.h"

void testBilayer() {
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
    string xmlContent = ss.str();
    file.newFromXMLContent(xmlContent,outputFileName);
    file.close();

    cerr << "parsing...." << endl;
    XMLParser parser;
    parser.setXMLContent(ss.str());
    parser.parseAll();

    Simulation *sim = parser.simulation();
    sim->setNWalkers(100000);
    sim->setNThreads(4);
    sim->setOutputFileName(outputFileName);

    sim->run();

    delete sim;
}
