#include "tests.h"

#include <iostream>

using namespace std;
using namespace MCPP;

const char outputFileName[] = "testCounters.h5";

void pass() {
    cout << "testCounters PASSED" << endl;
    remove(outputFileName);
    exit(EXIT_SUCCESS);
}

void fail() {
    cout << "FAILED" << endl;
    remove(outputFileName);
    exit(EXIT_FAILURE);
}

int main() {
    remove(outputFileName);
    testBilayer(outputFileName);

    H5OutputFile file;

    file.openFile(outputFileName);

    if(file.transmitted() != 56838) fail();
    if(file.ballistic() != 0) fail();
    if(file.reflected() != 903189) fail();
    if(file.backReflected() != 39973) fail();

    pass();
    return 0;
}

