#include "tests.h"

#include <iostream>

using namespace std;
using namespace MCPP;

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

    testBilayer();

    H5OutputFile file;

    file.openFile(outputFileName);

    if(file.transmitted() != 5768) fail();
    if(file.ballistic() != 0) fail();
    if(file.reflected() != 90327) fail();
    if(file.backReflected() != 3905) fail();

    pass();
    return 0;
}

