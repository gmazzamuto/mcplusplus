#include <stdio.h>
#include "h5outputfile.h"

#include <unistd.h>
#include <cmath>

#include <boost/math/constants/constants.hpp>

using namespace boost::math::constants;
using namespace std;

const char *progName;
MCfloat minVal[2], maxVal[2];
size_t nBins[2] = {1,1};
size_t binStride[2] = {1,1};
MCfloat **histoData[2];
MCfloat *data[2][4];
MCfloat binSize[] = {-1, -1};
MCfloat *modules[4] = {NULL, NULL, NULL, NULL};
const u_int64_t *photonCounters;
unsigned int wFlags = FLAG_TRANSMITTED | FLAG_BALLISTIC;
uint entriesPerWalker[2] = {0, 0};

void usage(FILE *f) {
    fprintf(f, "\n"
            "usage: %s [options] inputFile.h5 [data] [data2]\n"
            "\n"
            "where data and data2 can be one of the following strings:\n"
            "\t times\n"
            "\t kz\n"
            "\t points\n"
            "\n"
            "\tdata is mandatory\n"
            "\tdata2 is optional and specifies binning in the 2nd dimension\n\n"
            "[options]:\n"
            "\t -h display help\n"
            "\t -b [size] bin size (in deg for kz)\n"
            "\t -c [size] bin size for 2nd dimension (in deg for kz)\n"
            "\t -m [M] print only one bin every M bins\n"
            "\t -n [N] print only one bin every N bins (2nd dim)\n"
            "\t -t [tbrk] walker types (defaults to tb)\n"
            "\n", progName);
}

void preprocessData(DataGroup dataGroup, int dimension) {
    MCfloat **_data = data[dimension];

    if(dataGroup == DATA_TIMES) {
        if(binSize[dimension] < 0)
            binSize[dimension] = 5e-2;

        MCfloat minTemp=1./0., maxTemp=-1./0.;

        //find min and max values
        for (uint type = 0; type < 4; ++type) {
            if(_data[type] == NULL)
                continue;
            for (u_int64_t i = 0; i < photonCounters[type]; ++i) {
                if(_data[type][i] > maxTemp)
                    maxTemp = _data[type][i];
                if(_data[type][i] < minTemp)
                    minTemp = _data[type][i];
            }
        }

        maxVal[dimension] = maxTemp;
        minVal[dimension] = minTemp;
        nBins[dimension] = ceil((maxTemp - minTemp)/binSize[dimension]);
        histoData[dimension] = _data;
    }


    if(dataGroup == DATA_K) {

        minVal[dimension] = 90;
        maxVal[dimension] = 90;

        if(_data[TRANSMITTED] != NULL || _data[BALLISTIC] != NULL) {
            minVal[dimension] = 0;
        }

        if(_data[REFLECTED] != NULL || _data[BACKREFLECTED] != NULL) {
            maxVal[dimension] = 180;
        }

        nBins[dimension] = (maxVal[dimension]-minVal[dimension])/binSize[dimension];

        histoData[dimension] = _data;
    }

    if(dataGroup == DATA_POINTS) {
        for (uint type = 0; type < 4; type++) {
            if(wFlags & walkerTypeToFlag(type)) {
                modules[type] = (MCfloat*)malloc(sizeof(MCfloat)*photonCounters[type]);
            }
        }

        //compute modules
        for (uint type = 0; type < 4; ++type) {
            if(_data[type] == NULL)
                continue;
            u_int64_t j = 0;
            for (u_int64_t i = 0; i < photonCounters[type]; ++i) {
                modules[type][i] = sqrt(_data[type][j]*_data[type][j] + _data[type][j+1]*_data[type][j+1]);
                j+=2;
            }
        }

        MCfloat tempMax=-1./0.;

        if(binSize[dimension] < 0)
            binSize[dimension] = 5e-2;

        //find max value
        for (uint type = 0; type < 4; ++type) {
            if(_data[type] == NULL)
                continue;
            for (u_int64_t i = 0; i < photonCounters[type]; ++i) {
                if(modules[type][i] > tempMax) {
                    tempMax = modules[type][i];
                    if(tempMax > 10000) {
                        tempMax = 10000;
                        type = 5; //break outer loop
                        break;
                    }
                }
            }
        }

        minVal[dimension] = 0;
        maxVal[dimension] = tempMax;
        nBins[dimension] = ceil(tempMax/binSize[dimension]);
        histoData[dimension] = modules;
    }
}

int main(int argc, char *argv[])
{
    bool histo2D = false;

    progName = argv[0];

    //parse command line options
    char c;
    extern char *optarg;
    extern int optind;
    while ((c = getopt(argc, argv, "hb:c:t:m:n:")) != -1) {
        switch (c) {
        case 'h':
            usage(stdout);
            exit(EXIT_SUCCESS);
            break;

        case 'b':
            binSize[0] = atof(optarg);
            break;

        case 'c':
            binSize[1] = atof(optarg);
            break;

        case 'm':
            binStride[0] = atoi(optarg);
            break;

        case 'n':
            binStride[1] = atoi(optarg);
            break;

        case 't':
        {
            XMLParser parser;
            wFlags = parser.walkerSaveFlags(optarg);
        }
            break;

        default:
            break;
        }
    }

    if(argc-optind < 2) {
        fprintf(stderr,"Error: mandatory arguments not specified\n");
        usage(stderr);
        exit(EXIT_FAILURE);
    }

    const char *fileName = argv[optind++];
    DataGroup dataGroup[2];
    const char *dataGroupString = argv[optind++];
    if(strcmp(dataGroupString,"times") == 0)
        dataGroup[0] = DATA_TIMES;
    else if (strcmp(dataGroupString,"kz") == 0)
        dataGroup[0] = DATA_K;
    else if (strcmp(dataGroupString,"points") == 0)
        dataGroup[0] = DATA_POINTS;
    else {
        fprintf(stderr,"Error: unknown data group: %s\n",dataGroupString);
        usage(stderr);
        exit(EXIT_FAILURE);
    }

    if(argc-optind != 0) {
        dataGroupString = argv[optind++];
        if(strcmp(dataGroupString,"times") == 0)
            dataGroup[1] = DATA_TIMES;
        else if (strcmp(dataGroupString,"kz") == 0)
            dataGroup[1] = DATA_K;
        else if (strcmp(dataGroupString,"points") == 0)
            dataGroup[1] = DATA_POINTS;
        else {
            fprintf(stderr,"Error: unknown data group: %s\n",dataGroupString);
            usage(stderr);
            exit(EXIT_FAILURE);
        }
        histo2D = true;
    }

    if(dataGroup[0] == dataGroup[1]) {
        fprintf(stderr,"Error: data and data2 cannot be the same\n");
        exit(EXIT_FAILURE);
    }

    //end options

    H5OutputFile file;
    if(!file.openFile(fileName)) {
        cerr << "Cannot open file " << fileName << endl;
    }

    photonCounters = file.photonCounters();

    cerr << "Walker types: ";
    for (uint type = 0; type < 4; ++type) {
        if(wFlags & walkerTypeToFlag(type))
            cerr << walkerTypeToString(type) << " ";
    }

    cerr << endl;

    for (int d = 0; d < 2; ++d) {
        switch (dataGroup[d]) {
        case DATA_TIMES:
            entriesPerWalker[d] = 1;
            break;
        case DATA_POINTS:
            entriesPerWalker[d] = 2;
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
                    entriesPerWalker[d]++;
            }
        }
        default:
            break;
        }
    }

    //allocate memory and load data from file
    for (int i = 0; i < 2; ++i) {
        memset(data[i],0,4*sizeof(MCfloat *));
        for (uint type = 0; type < 4; type++) {
            if(wFlags & walkerTypeToFlag(type)) {
                data[i][type] = (MCfloat*)malloc(sizeof(MCfloat)*photonCounters[type]*entriesPerWalker[i]);
                if(!file.loadData((DataGroup)dataGroup[i],(walkerType)type, data[i][type])) {
                    free(data[i][type]);
                    data[i][type] = NULL;
                }
            }
        }
        if(i == 0 && !histo2D)
            break;
    }
    preprocessData(dataGroup[0], 0);
    if(histo2D)
        preprocessData(dataGroup[1], 1);

    for (int i = 0; i < 2; ++i) {
        cerr << "dim " << i << " nbins = " << nBins[i] << " minVal = " << minVal[i] << endl;
    }

    //build histogram
    MCfloat scale1 = (photonCounters[0]+photonCounters[1]+photonCounters[2]+photonCounters[3]);
    size_t nStridedBins[2];
    nStridedBins[0] = nBins[0] / binStride[0];  //check rounding
    nStridedBins[1] = nBins[1] / binStride[1];
    size_t totBins = nStridedBins[0]*nStridedBins[1];
    u_int64_t *histo = (u_int64_t *)calloc(totBins,sizeof(u_int64_t));
    MCfloat degPerRad = 180/pi<MCfloat>();
    for (uint type = 0; type < 4; ++type) {
        if(histoData[0][type] == NULL)
            continue;
        for (u_int64_t i = 0; i < photonCounters[type]; ++i) {
            size_t index[2] = {0,0};
            if(dataGroup[0] == DATA_K)
                index[0] = (acos(histoData[0][type][i])*degPerRad - minVal[0])/binSize[0];
            else
                index[0] = (histoData[0][type][i]-minVal[0])/binSize[0];

            if(index[0] >= nBins[0])
                continue;

            if(index[0] % binStride[0] != 0)
                continue;

            if(histo2D) {
                if(dataGroup[1] == DATA_K)
                    index[1] = (acos(histoData[1][type][i])*degPerRad - minVal[1])/binSize[1];
                else
                    index[1] = (histoData[1][type][i]-minVal[1])/binSize[1];
                if(index[1] >= nBins[1])
                    continue;
                if(index[1] % binStride[1] != 0)
                    continue;
            }

            size_t idx =  (index[0]/binStride[0])*nStridedBins[1] + index[1]/binStride[1];
            histo[idx]++;
        }
    }

    //print histogram

    switch (dataGroup[0]) {
    case DATA_K:
        cout << "cosTheta";
        break;

    case DATA_POINTS:
        cout << "um";
        break;

    case DATA_TIMES:
        cout << "ps";
        break;
    default:
        break;
    }
    if(histo2D)
        for (unsigned int i = 0; i < nBins[1]; ++i) {
            if(i % binStride[1] != 0)
                continue;
            if(dataGroup[1] == DATA_K)
                cout << "\tb-" << cos(binSize[1]*(i+0.5));
            else
                cout << "\tb-" << binSize[1]*(i+0.5);
        }
    else
        cout << "\tcounts";

    cout << endl;


    switch (dataGroup[0]) {
    case DATA_K:
        for (unsigned int i = 0; i < nBins[0]; ++i) {
            if(i % binStride[0] != 0)
                continue;
            MCfloat binCenter = cos(binSize[0]*(i+0.5)/degPerRad);
            MCfloat scale2 = scale1*4.0*pi<MCfloat>()*sin((i+0.5)*binSize[0]/degPerRad)*sin(binSize[0]/2./degPerRad);
            cout << binCenter;
            MCfloat lastBinCenter = (binSize[0]*((nBins[0]-1)+0.5));
            for (unsigned int j = 0; j < nBins[1]; ++j) {
                if(j % binStride[1] != 0)
                    continue;
                cout << "\t" << histo[(i/binStride[0])*nStridedBins[1] + j/binStride[1]]/scale2;
            }
            cout << endl;
        }
        break;

    case DATA_TIMES:
        for (unsigned int i = 0; i < nBins[0]; ++i) {
            if(i % binStride[0] != 0)
                continue;
            MCfloat binCenter = binSize[0]*(i+0.5);
            MCfloat scale2 = scale1;
            cout << binCenter;
            for (unsigned int j = 0; j < nBins[1]; ++j) {
                if(j % binStride[1] != 0)
                    continue;
                cout << "\t" << histo[(i/binStride[0])*nStridedBins[1] + j/binStride[1]]/scale2;
            }
            cout << endl;
        }
        break;

    case DATA_POINTS:
        for (unsigned int i = 0; i < nBins[0]; ++i) {
            if(i % binStride[0] != 0)
                continue;
            MCfloat dr = binSize[0];
            MCfloat binCenter = binSize[0]*(i+0.5);
            MCfloat scale2 = scale1 * (2*pi<MCfloat>()*(i+0.5)*dr*dr);
            cout << binCenter;
            for (unsigned int j = 0; j < nBins[1]; ++j) {
                if(j % binStride[1] != 0)
                    continue;
                cout << "\t" << histo[(i/binStride[0])*nStridedBins[1] + j/binStride[1]]/scale2;
            }
            cout << endl;
        }
        break;

    default:
        break;
    }
    return 0;






    //    if(dataGroup == DATA_TIMES) {
    //        if(binSize < 0)
    //            binSize = 5e-2;

    //        MCfloat min=1./0., max=-1./0.;

    //        //find min and max values
    //        for (uint type = 0; type < 4; ++type) {
    //            if(data[type] == NULL)
    //                continue;
    //            for (u_int64_t i = 0; i < photonCounters[type]; ++i) {
    //                if(data[type][i] > max)
    //                    max = data[type][i];
    //                if(data[type][i] < min)
    //                    min = data[type][i];
    //            }
    //        }

    //        if(min > 0)
    //            min = 0;

    //        size_t nBins = ceil((max - min)/binSize);

    //        cerr << "bin size: " << binSize << endl;
    //        cerr << "max = " << max << " min = " << min << endl;
    //        cerr << "nBins = " << nBins << endl;

    //        //build histogram
    //        u_int64_t histo[nBins];
    //        memset(histo,0,nBins*sizeof(u_int64_t));

    //        for (uint type = 0; type < 4; ++type) {
    //            if(data[type] == NULL)
    //                continue;
    //            for (u_int64_t i = 0; i < photonCounters[type]; ++i) {
    //                unsigned int index = (data[type][i]-min)/binSize;
    //                histo[index]++;
    //            }
    //        }

    //        cout << "binCenter\tcounts" << endl;

    //        for (size_t i = 0; i < nBins; ++i) {
    //            cout << min + binSize*(i+0.5) << "\t" << histo[i];
    //            cout << endl;
    //        }
    //    }

    //    if(dataGroup == DATA_K) {
    //        size_t nBins;

    //        if(binSize < 0)  //here binSize is the value of -b flag
    //            nBins = 100;
    //        else
    //            nBins = binSize;


    //        binSize = 1./nBins;

    //        cerr << "bin size: " << binSize << endl;
    //        cerr << "nBins = " << nBins << endl;

    //        //build histogram
    //        u_int64_t histok[nBins];
    //        memset(histok,0,nBins*sizeof(u_int64_t));

    //        for (uint type = 0; type < 4; ++type) {
    //            if(data[type] == NULL)
    //                continue;
    //            for (u_int64_t i = 0; i < photonCounters[type]; ++i) {
    //                unsigned int index = data[type][i]/binSize;
    //                histok[index]++;
    //            }
    //        }

    //        u_int64_t histoa[nBins];
    //        memset(histoa,0,nBins*sizeof(u_int64_t));
    //        MCfloat binSizea = pi<MCfloat>()/2./nBins;

    //        for (uint type = 0; type < 4; ++type) {
    //            if(data[type] == NULL)
    //                continue;
    //            for (u_int64_t i = 0; i < photonCounters[type]; ++i) {
    //                unsigned int index = acos(data[type][i])/binSizea;
    //                if(index > nBins - 1) { //see mcml (??)
    //                    cerr << "Warning: index = " << index << endl;
    //                    index = nBins - 1;
    //                }
    //                histoa[index]++;
    //            }
    //        }

    //        cout << "kz\tP(kz)/kz\tcounts\tcostheta\tcounts\tscaled" << endl;

    //        MCfloat scale1 = photonCounters[0]+photonCounters[1]+photonCounters[2]+photonCounters[3];
    //        MCfloat lastBinCenter = (binSize*((nBins-1)+0.5));
    //        for (size_t i = 0; i < nBins; ++i) {
    //            MCfloat binCenter = binSize*(i+0.5);
    //            cout << binCenter << "\t" << (histok[i]/binCenter/histok[nBins-1])*lastBinCenter*(2./5.+3./5.*lastBinCenter);
    //            cout << "\t" << histok[i];

    //            cout << "\t" << cos(binSizea*(i+0.5));
    //            cout << "\t" << histoa[i];
    //            MCfloat scale2 = scale1*4.0*pi<MCfloat>()*sin((i+0.5)*binSizea)*sin(binSizea/2.);
    //            cout << "\t" << histoa[i] / scale2;

    //            cout << endl;
    //        }
    //    }

    //    if(dataGroup == DATA_POINTS) {
    //        MCfloat *modules[4];
    //        memset(modules,0,4*sizeof(MCfloat *));
    //        for (uint type = 0; type < 4; type++) {
    //            if(walkerFlags & walkerTypeToFlag(type)) {
    //                modules[type] = (MCfloat*)malloc(sizeof(MCfloat)*photonCounters[type]);
    //            }
    //        }

    //        //compute modules
    //        for (uint type = 0; type < 4; ++type) {
    //            if(data[type] == NULL)
    //                continue;
    //            u_int64_t j = 0;
    //            for (u_int64_t i = 0; i < photonCounters[type]; ++i) {
    //                modules[type][i] = sqrt(data[type][j]*data[type][j] + data[type][j+1]*data[type][j+1]);
    //                j+=2;
    //            }
    //        }

    //        MCfloat max=-1./0.;

    //        if(binSize < 0)
    //            binSize = 5e-2;

    //        //find max value
    //        for (uint type = 0; type < 4; ++type) {
    //            if(data[type] == NULL)
    //                continue;
    //            for (u_int64_t i = 0; i < photonCounters[type]; ++i) {
    //                if(modules[type][i] > max) {
    //                    max = modules[type][i];
    //                    if(max > 10000) {
    //                        max = 10000;
    //                        type = 5; //break outer loop
    //                        break;
    //                    }
    //                }
    //            }
    //        }

    //        size_t nBins = ceil(max/binSize);

    //        cerr << "bin size: " << binSize << endl;
    //        cerr << "max = " << max << " min = 0" << endl;
    //        cerr << "nBins = " << nBins << endl;

    //        //build histogram
    //        u_int64_t *histo = (u_int64_t*)calloc(nBins, sizeof(u_int64_t));
    //        if(histo == NULL) {
    //            cerr << "Cannot allocate memory" << endl;
    //            exit(EXIT_FAILURE);
    //        }
    //        for (uint type = 0; type < 4; ++type) {
    //            if(data[type] == NULL)
    //                continue;
    //            for (u_int64_t i = 0; i < photonCounters[type]; ++i) {
    //                uint index = modules[type][i]/binSize;
    //                histo[index]++;
    //            }
    //        }

    //        cout << "binCenter\tintensity" << endl;

    //        MCfloat dr = binSize;
    //        MCfloat scale1 = (photonCounters[0]+photonCounters[1]+photonCounters[2]+photonCounters[3]);
    //        for (int i = 0; i < nBins; ++i) {
    //            MCfloat r = binSize*i;
    //            MCfloat scale2 = scale1 * (2*pi<MCfloat>()*(i+0.5)*dr*dr);
    //            MCfloat intensity = histo[i] / scale2;
    //            cout << r + dr/2 << "\t" << intensity;
    //            cout << endl;
    //        }

    //        for (uint type = 0; type < 4; ++type) {
    //            if(modules[type] == NULL)
    //                continue;
    //            free(modules[type]);
    //        }

    //        free(histo);
    //    }



    //free allocated memory
    //    for (uint type = 0; type < 4; ++type) {
    //        if(data[type] == NULL)
    //            continue;
    //        free(data[type]);
    //    }

    return 0;
}

