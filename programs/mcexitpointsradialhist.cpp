#include <stdio.h>
#include "h5outputfile.h"

#include <cmath>

#include <boost/math/constants/constants.hpp>

using namespace boost::math::constants;
using namespace std;

int main(int argc, char *argv[])
{
    H5OutputFile file;
    file.openFile(argv[1]);
    u_int64_t transmitted = file.transmitted();

    MCfloat *ep = (MCfloat*)malloc(sizeof(MCfloat)*2*transmitted);
    file.loadExitPoints(Simulation::TRANSMITTED, ep);

    MCfloat *modules = (MCfloat*)malloc(sizeof(MCfloat)*transmitted);

    u_int64_t j = 0;
    for (u_int64_t i = 0; i < transmitted; ++i) {
        modules[i] = sqrt(ep[j]*ep[j] + ep[j+1]*ep[j+1]);
        j+=2;
    }

    MCfloat max=-1./0.;
    MCfloat binSize = 5e-2;

    //find max value
    for (u_int64_t i = 0; i < transmitted; ++i) {
        if(modules[i] > max)
            max = modules[i];
    }

    size_t nBins = ceil(max/binSize);

    u_int64_t histo[nBins];
    memset(histo,0,nBins*sizeof(u_int64_t));

    for (u_int64_t i = 0; i < transmitted; ++i) {
        uint index = modules[i]/binSize;
        histo[index]++;
    }

    cout << "binCenter\tintensity\tlogIntensity" << endl;

    MCfloat dr = binSize;
    for (int i = 0; i < nBins; ++i) {
        MCfloat r = binSize*i;
        MCfloat intensity = histo[i] / (pi<MCfloat>()*(2*r*dr+dr*dr));
        cout << r + dr/2 << "\t" << intensity;
        if(intensity!=0)
            cout << "\t" << log10(intensity);
        cout << endl;
    }

    free(ep);
    free(modules);



    return 0;
}


