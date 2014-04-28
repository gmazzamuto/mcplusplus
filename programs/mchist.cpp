#include <stdio.h>
#include "h5outputfile.h"

#include <unistd.h>

#include <boost/math/constants/constants.hpp>

using namespace boost::math::constants;
using namespace std;

const char *progName;

void usage(FILE *f) {
    fprintf(f, "\n"
            "usage: %s [options] inputFile.h5 [data]\n"
            "\n"
            "where data can be one of the following strings:\n"
            "\t times\n"
            "\t kz\n"
            "\t points\n"
            "\n"
            "[options]:\n"
            "\t -h display help\n"
            "\t -b [size] binning\n"
            "\t           (bin size for times and points, # of bins for kz)\n"
            "\t -t [tbrk] walker types (defaults to tb)\n"
            "\n", progName);
}

int main(int argc, char *argv[])
{
    MCfloat binSize = -1;

    progName = argv[0];

    unsigned int walkerFlags = FLAG_TRANSMITTED | FLAG_BALLISTIC;

    //parse command line options
    char c;
    extern char *optarg;
    extern int optind;
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

    if(argc-optind != 2) {
        fprintf(stderr,"Error: mandatory arguments not specified\n");
        usage(stderr);
        exit(EXIT_FAILURE);
    }

    const char *fileName = argv[optind++];
    const char *dataGroupString = argv[optind++];
    DataGroup dataGroup;
    if(strcmp(dataGroupString,"times") == 0)
        dataGroup = DATA_TIMES;
    else if (strcmp(dataGroupString,"kz") == 0)
        dataGroup = DATA_K;
    else if (strcmp(dataGroupString,"points") == 0)
        dataGroup = DATA_POINTS;
    else {
        fprintf(stderr,"Error: unknown data group: %s\n",dataGroupString);
        usage(stderr);
        exit(EXIT_FAILURE);
    }

    //end options

    H5OutputFile file;
    if(!file.openFile(fileName)) {
        cerr << "Cannot open file " << fileName << endl;
    }

    const u_int64_t *photonCounters = file.photonCounters();

    cerr << "Walker types: ";
    for (uint type = 0; type < 4; ++type) {
        if(walkerFlags & walkerIndexToFlag(type))
            cerr << walkerIndexToString(type) << " ";
    }

    cerr << endl;

    uint entriesPerWalker = 0;
    switch (dataGroup) {
    case DATA_TIMES:
        entriesPerWalker = 1;
        break;
    case DATA_POINTS:
        entriesPerWalker = 2;
        break;
    case DATA_K: {
        XMLParser parser;
        parser.setXMLContent(file.readXMLDescription());
        parser.parseOutput();
        if(!(parser.exitKVectorsDirsSaveFlags() & DIR_Z))
        {
            fprintf(stderr,"Error: file does not contain kz data\n");
            usage(stderr);
            exit(EXIT_FAILURE);
        }
        for (uint i = 0; i < 3; ++i) {
            if(parser.exitKVectorsDirsSaveFlags() & (1 << i))
                entriesPerWalker++;
        }
    }
    default:
        break;
    }

    //allocate memory and load data from file
    MCfloat *data[4];
    memset(data,0,4*sizeof(MCfloat *));
    for (uint type = 0; type < 4; type++) {
        if(walkerFlags & walkerIndexToFlag(type)) {
            data[type] = (MCfloat*)malloc(sizeof(MCfloat)*photonCounters[type]*entriesPerWalker);
            if(!file.loadData((DataGroup)dataGroup,(walkerIndex)type, data[type])) {
                free(data[type]);
                data[type] = NULL;
            }
        }
    }




    if(dataGroup == DATA_TIMES) {
        if(binSize < 0)
            binSize = 5e-2;

        MCfloat min=1./0., max=-1./0.;

        //find min and max values
        for (uint type = 0; type < 4; ++type) {
            if(data[type] == NULL)
                continue;
            for (u_int64_t i = 0; i < photonCounters[type]; ++i) {
                if(data[type][i] > max)
                    max = data[type][i];
                if(data[type][i] < min)
                    min = data[type][i];
            }
        }

        if(min > 0)
            min = 0;

        size_t nBins = ceil((max - min)/binSize);

        cerr << "bin size: " << binSize << endl;
        cerr << "max = " << max << " min = " << min << endl;
        cerr << "nBins = " << nBins << endl;

        //build histogram
        u_int64_t histo[nBins];
        memset(histo,0,nBins*sizeof(u_int64_t));

        for (uint type = 0; type < 4; ++type) {
            if(data[type] == NULL)
                continue;
            for (u_int64_t i = 0; i < photonCounters[type]; ++i) {
                unsigned int index = (data[type][i]-min)/binSize;
                histo[index]++;
            }
        }

        cout << "binCenter\tcounts" << endl;

        for (size_t i = 0; i < nBins; ++i) {
            cout << min + binSize*(i+0.5) << "\t" << histo[i];
            cout << endl;
        }
    }

    if(dataGroup == DATA_K) {
        size_t nBins;

        if(binSize < 0)  //here binSize is the value of -b flag
            nBins = 100;
        else
            nBins = binSize;


        binSize = 1./nBins;

        cerr << "bin size: " << binSize << endl;
        cerr << "nBins = " << nBins << endl;

        //build histogram
        u_int64_t histo[nBins];
        memset(histo,0,nBins*sizeof(u_int64_t));

        for (uint type = 0; type < 4; ++type) {
            if(data[type] == NULL)
                continue;
            for (u_int64_t i = 0; i < photonCounters[type]; ++i) {
                unsigned int index = data[type][i]/binSize;
                histo[index]++;
            }
        }

        cout << "binCenter\tP(kz)" << endl;

        MCfloat lastBinCenter = (binSize*((nBins-1)+0.5));
        for (size_t i = 0; i < nBins; ++i) {
            MCfloat binCenter = binSize*(i+0.5);
            cout << binCenter << "\t" << (histo[i]/binCenter/histo[nBins-1])*lastBinCenter*(2./5.+3./5.*lastBinCenter) << "\t" << histo[i];
            cout << endl;
        }
    }

    if(dataGroup == DATA_POINTS) {
        MCfloat *modules[4];
        memset(modules,0,4*sizeof(MCfloat *));
        for (uint type = 0; type < 4; type++) {
            if(walkerFlags & walkerIndexToFlag(type)) {
                modules[type] = (MCfloat*)malloc(sizeof(MCfloat)*photonCounters[type]);
            }
        }

        //compute modules
        for (uint type = 0; type < 4; ++type) {
            if(data[type] == NULL)
                continue;
            u_int64_t j = 0;
            for (u_int64_t i = 0; i < photonCounters[type]; ++i) {
                modules[type][i] = sqrt(data[type][j]*data[type][j] + data[type][j+1]*data[type][j+1]);
                j+=2;
            }
        }

        MCfloat max=-1./0.;

        if(binSize < 0)
            binSize = 5e-2;

        //find max value
        for (uint type = 0; type < 4; ++type) {
            if(data[type] == NULL)
                continue;
            for (u_int64_t i = 0; i < photonCounters[type]; ++i) {
                if(modules[type][i] > max)
                    max = modules[type][i];
            }
        }

        size_t nBins = ceil(max/binSize);

        cerr << "bin size: " << binSize << endl;
        cerr << "max = " << max << " min = 0" << endl;
        cerr << "nBins = " << nBins << endl;

        //build histogram
        u_int64_t histo[nBins];
        memset(histo,0,nBins*sizeof(u_int64_t));
        for (uint type = 0; type < 4; ++type) {
            if(data[type] == NULL)
                continue;
            for (u_int64_t i = 0; i < photonCounters[type]; ++i) {
                uint index = modules[type][i]/binSize;
                histo[index]++;
            }
        }

        cout << "binCenter\tintensity" << endl;

        MCfloat dr = binSize;
        for (int i = 0; i < nBins; ++i) {
            MCfloat r = binSize*i;
            MCfloat intensity = histo[i] / (pi<MCfloat>()*(2*r*dr+dr*dr));
            cout << r + dr/2 << "\t" << intensity;
            cout << endl;
        }

        for (uint type = 0; type < 4; ++type) {
            if(modules[type] == NULL)
                continue;
            free(modules[type]);
        }
    }



    //free allocated memory
    for (uint type = 0; type < 4; ++type) {
        if(data[type] == NULL)
            continue;
        free(data[type]);
    }

    return 0;
}

