#include <stdio.h>
#include "h5outputfile.h"

#include <iostream>
#include <cmath>

#include <boost/math/constants/constants.hpp>

using namespace boost::math::constants;
using namespace std;

int main(int argc, char *argv[])
{
    H5OutputFile file;
    file.openFile(argv[1]);
    size_t transmitted = file.transmitted();

    MCfloat *ek = (MCfloat*)malloc(sizeof(MCfloat)*transmitted);
    file.loadExitKVectors(Simulation::TRANSMITTED, ek);
    file.close();

    MCfloat binSize = 1e-2;

    size_t nBins = ceil(1./binSize);

    u_int64_t histo[nBins];
    memset(histo,0,nBins*sizeof(u_int64_t));

    for (size_t i = 0; i < transmitted; ++i) {
        size_t index = ek[i]/binSize;
        histo[index]++;
    }

    cout << "binCenter\tP(kz)" << endl;

    for (size_t i = 0; i < nBins; ++i) {
        MCfloat binCenter = binSize*(i+0.5);
        cout << binCenter << "\t" << histo[i]/binCenter/histo[nBins-1];
        cout << endl;
    }

    free(ek);

    return 0;
}
