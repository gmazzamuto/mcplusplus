#include "tests.h"

#include <iostream>

void pass() {
    cout << "testH5Append PASSED" << endl;
    remove(outputFileName);
    exit(EXIT_SUCCESS);
}

void fail() {
    cout << "FAILED" << endl;
    remove(outputFileName);
    exit(EXIT_FAILURE);
}

int main() {
    testBilayer();

    H5OutputFile *file = new H5OutputFile();
    file->setXMLParserEnabled(true);
    file->openFile(outputFileName);

    Simulation *sim = file->simulation();

    sim->setNWalkers(100000);
    sim->setNThreads(4);

    sim->run();

    delete sim;
    file->close();
    delete file;

    file = new H5OutputFile();
    file->openFile(outputFileName);

    if(file->transmitted() != 11536) fail();
    if(file->ballistic() != 0) fail();
    if(file->reflected() != 180589) fail();
    if(file->backReflected() != 7875) fail();

    pass();
    return 0;
}

