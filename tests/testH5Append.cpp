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
    //FIXME
    fail();
//    testBilayer();

//    H5OutputFile *file = new H5OutputFile();
//    file->openFile(outputFileName);

//    Simulation *sim = file->simulation();

//    sim->setNWalkers(100000);
//    sim->setNThreads(4);

//    sim->run();

//    delete sim;
//    file->close();
//    delete file;

//    file = new H5OutputFile();
//    file->openFile(outputFileName);

//    if(file->transmitted() != 11537) fail();
//    if(file->ballistic() != 0) fail();
//    if(file->reflected() != 180585) fail();
//    if(file->backReflected() != 7878) fail();

//    pass();
    return 0;
}

