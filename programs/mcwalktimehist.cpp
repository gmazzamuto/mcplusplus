#include <stdio.h>
#include "h5outputfile.h"

#include <cmath>
#include <unistd.h>

using namespace std;

const char *progName;
MCfloat binSize = 5e-2;

void usage(FILE *f) {
    fprintf(f, "\n"
            "usage: %s [options] inputFile.h5\n"
            "\n"
            "[options]:\n"
            "\t -h display help\n"
            "\t -b [size] bin size in ps (defaults to %f)\n"
            "\t -t [tbrk] walker types (defaults to tb)\n"
            "\n", progName, binSize);
}

int main(int argc, char *argv[])
{
    progName = argv[0];

    const char *fileName;
    unsigned int walkerFlags = FLAG_TRANSMITTED | FLAG_BALLISTIC;

    char c;
    extern char *optarg;
    extern int optind;

    MCfloat *wt[4];
    memset(wt,0,4*sizeof(MCfloat *));

    const u_int64_t *photonCounters;

    while ((c = getopt(argc, argv, "hs:t:")) != -1) {
        switch (c) {
        case 'h':
            usage(stdout);
            exit(EXIT_SUCCESS);
            break;

        case 's':
            binSize = atof(optarg);
            break;

        case 't':
        {
            XMLParser parser;
            walkerFlags = parser.walkerSaveFlags(optarg);
        }
            break;

        default:
            break;
        }
    }

    if(argc-optind != 1) {
        fprintf(stderr,"Error: input file not specified\n");
        usage(stderr);
        exit(EXIT_FAILURE);
    }

    fileName = argv[optind++];

    H5OutputFile file;
    if(!file.openFile(fileName)) {
        cerr << "Cannot open file " << fileName << endl;
    }

    photonCounters = file.photonCounters();

    cerr << "Bin size: " << binSize << endl;
    cerr << "Walker types: ";
    if(walkerFlags & FLAG_TRANSMITTED)
        cerr << "TRANSMITTED ";
    if(walkerFlags & FLAG_BALLISTIC)
        cerr << "BALLISTIC ";
    if(walkerFlags & FLAG_REFLECTED)
        cerr << "REFLECTED ";
    if(walkerFlags & FLAG_BACKREFLECTED)
        cerr << "BACKREFLECTED ";

    cerr << endl;

    //allocate memory
    for (uint type = 0; type < 4; type++) {
        if(walkerFlags & walkerIndexToFlag(type)) {
            wt[type] = (MCfloat*)malloc(sizeof(MCfloat)*photonCounters[type]);
            if(!file.loadWalkTimes((walkerIndex)type, wt[type])) {
                free(wt[type]);
                wt[type] = NULL;
            }
        }
    }

    MCfloat min=1./0., max=-1./0.;

    //find min and max values
    for (uint type = 0; type < 4; ++type) {
        if(wt[type] == NULL)
            continue;
        for (u_int64_t i = 0; i < photonCounters[type]; ++i) {
            if(wt[type][i] > max)
                max = wt[type][i];
            if(wt[type][i] < min)
                min = wt[type][i];
        }
    }

    if(min > 0)
        min = 0;

    size_t nBins = ceil((max - min)/binSize);

    cerr << "max = " << max << " min = " << min << endl;
    cerr << "nBins = " << nBins << endl;

    u_int64_t *histo = (u_int64_t*)calloc(nBins,sizeof(u_int64_t));

    //build histogram
    for (uint type = 0; type < 4; ++type) {
        if(wt[type] == NULL)
            continue;
        for (u_int64_t i = 0; i < photonCounters[type]; ++i) {
            unsigned int index = (wt[type][i]-min)/binSize;
            histo[index]++;
        }

    }

    cout << "binCenter\tcounts\tlogCounts" << endl;

    for (size_t i = 0; i < nBins; ++i) {
        cout << min + binSize*(i+0.5) << "\t" << histo[i];
        if(histo[i])
            cout << "\t" << log10(histo[i]);
        cout << endl;
    }

    //free allocated memory
    for (uint type = 0; type < 4; ++type) {
        if(wt[type] == NULL)
            continue;
        free(wt[type]);
    }

    return 0;
}

