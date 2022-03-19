#include "tests.h"

#include <iostream>

using namespace std;
using namespace MCPP;

const char outputFileName[] = "testHistogram.h5";

void pass() {
    cout << "testHistogram PASSED" << endl;
    remove(outputFileName);
    exit(EXIT_SUCCESS);
}

void fail() {
    cout << "FAILED" << endl;
    remove(outputFileName);
    exit(EXIT_FAILURE);
}

int main() {

    testBilayer(outputFileName);

    H5OutputFile file;

    file.openFile(outputFileName);

    MCfloat buf[52*251];
    uint idx;
    MCfloat relativeError, result, expectedResult;

    file.openDataSet("points");
    file.loadAll(buf);
    idx = 29*2+1;
    result = buf[idx];
    expectedResult = 1.6050371379606394e-06;
    relativeError = fabs((result - expectedResult) / expectedResult);
    if(relativeError > 1e-13) fail();

    file.openDataSet("times");
    file.loadAll(buf);
    idx = 29*3+1;
    result = buf[idx];
    expectedResult = 0.000104;
    relativeError = fabs((result - expectedResult) / expectedResult);
    if(relativeError > 1e-13) fail();

    idx = 29*3+2;
    result = buf[idx];
    expectedResult = 15468.52147677016;
    relativeError = fabs((result - expectedResult) / expectedResult);
    if(relativeError > 1e-13) fail();

    file.openDataSet("k");
    file.loadAll(buf);
    idx = 16*2+1;
    result = buf[idx];
    expectedResult = 0.0012899118311509026;
    relativeError = fabs((result - expectedResult) / expectedResult);
    if(relativeError > 1e-13) fail();

    file.openDataSet("points_vs_time");
    file.loadAll(buf);
    idx = 20*52+10;
    result = buf[idx];
    expectedResult = 1.9991413583494172e-07;
    relativeError = fabs((result - expectedResult) / expectedResult);
    if(relativeError > 1e-13) fail();

    pass();
    return 0;
}

