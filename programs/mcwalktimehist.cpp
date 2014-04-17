#include <stdio.h>
#include "h5outputfile.h"

#include <iostream>
#include <cmath>

using namespace std;

int main(int argc, char *argv[])
{
    H5OutputFile file;
    file.openFile(argv[1]);
    u_int64_t transmitted = file.transmitted();

    MCfloat *wt = (MCfloat*)malloc(sizeof(MCfloat)*transmitted);
    file.loadWalkTimes(Simulation::TRANSMITTED, wt);

    MCfloat min=1./0., max=-1./0.;
    MCfloat binSize = 5e-2;

    //find min and max values
    for (int i = 0; i < transmitted; ++i) {
        if(wt[i] > max)
            max = wt[i];
        if(wt[i] < min)
            min = wt[i];
    }

//    cout << "max = " << max << " min = " << min << endl;

    size_t nBins = ceil((max - min)/binSize);

    u_int64_t histo[nBins];
    memset(histo,0,nBins*sizeof(u_int64_t));

    for (int i = 0; i < transmitted; ++i) {
        unsigned int index = (wt[i]-min)/binSize;
        histo[index]++;
    }

    cout << "binCenter\tcounts\tlogCounts" << endl;

    for (int i = 0; i < nBins; ++i) {
        cout << min + binSize*(i+0.5) << "\t" << histo[i];
        if(histo[i])
            cout << "\t" << log10(histo[i]);
        cout << endl;
    }

    free(wt);



    return 0;
}

