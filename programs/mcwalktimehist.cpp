#include <stdio.h>
#include "h5outputfile.h"

#include <cmath>

using namespace std;

int main(int argc, char *argv[])
{
    H5OutputFile file;
    file.openFile(argv[1]);
    u_int64_t transmitted = file.transmitted();
    u_int64_t ballistic = file.ballistic();

    MCfloat *wt = (MCfloat*)malloc(sizeof(MCfloat)*transmitted);
    file.loadWalkTimes(Simulation::TRANSMITTED, wt);

    MCfloat *wtb = (MCfloat*)malloc(sizeof(MCfloat)*ballistic);
    file.loadWalkTimes(Simulation::BALLISTIC, wtb);

    MCfloat min=1./0., max=-1./0.;
    MCfloat binSize = 5e-2;

    //find min and max values
    for (int i = 0; i < transmitted; ++i) {
        if(wt[i] > max)
            max = wt[i];
        if(wt[i] < min)
            min = wt[i];
    }

    for (int i = 0; i < ballistic; ++i) {
        if(wtb[i] > max)
            max = wt[i];
        if(wtb[i] < min)
            min = wt[i];
    }

    if(min > 0)
        min = 0;

    size_t nBins = ceil((max - min)/binSize);

//    cout << "max = " << max << " min = " << min << endl;
//    cout << "nBins = " << nBins << endl;

    u_int64_t *histo = (u_int64_t*)calloc(nBins,sizeof(u_int64_t));

    for (int i = 0; i < transmitted; ++i) {
        unsigned int index = (wt[i]-min)/binSize;
        histo[index]++;
    }

    for (int i = 0; i < ballistic; ++i) {
        unsigned int index = (wtb[i]-min)/binSize;
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
    free(wtb);
    free(histo);



    return 0;
}

