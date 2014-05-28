#include "mcmoviecreator.h"

#include <QString>

#include "h5movie.h"

MCMovieCreator::MCMovieCreator(QString fileName, BaseObject *parent) :
    H5FileHelper(parent)
{
    file.openFile(fileName.toStdString().c_str());

    memset(dataTimes,0,4*sizeof(MCfloat *));
    memset(dataPoints,0,4*sizeof(MCfloat *));
    binSize = 5e-2;
    binSizeX = 5e-2;
    wFlags = FLAG_TRANSMITTED | FLAG_BALLISTIC;
    startFrame = 0;
    endFrame = 1;
}

void MCMovieCreator::createMovie(const QString fileName) {
    const u_int64_t *photonCounters = file.photonCounters();
    //allocate memory and load data from file
    for (uint type = 0; type < 4; type++) {
        if(wFlags & walkerTypeToFlag(type)) {
            dataTimes[type] = (MCfloat*)malloc(sizeof(MCfloat)*photonCounters[type]);
            if(!file.loadData(DATA_TIMES,(walkerType)type, dataTimes[type])) {
                free(dataTimes[type]);
                dataTimes[type] = NULL;
            }
        }
    }

    for (uint type = 0; type < 4; type++) {
        if(wFlags & walkerTypeToFlag(type)) {
            dataPoints[type] = (MCfloat*)malloc(sizeof(MCfloat)*photonCounters[type]*2);
            if(!file.loadData(DATA_POINTS,(walkerType)type, dataPoints[type])) {
                free(dataPoints[type]);
                dataPoints[type] = NULL;
            }
        }
    }

    MCfloat minVal=1./0., maxVal=-1./0.;

    //find min and max values
    for (uint type = 0; type < 4; ++type) {
        if(dataTimes[type] == NULL)
            continue;
        for (u_int64_t i = 0; i < photonCounters[type]; ++i) {
            if(dataTimes[type][i] > maxVal)
                maxVal = dataTimes[type][i];
            if(dataTimes[type][i] < minVal)
                minVal = dataTimes[type][i];
        }
    }

    unsigned int nBins = ceil((maxVal - minVal)/binSize);
    if(nBins == 0)
        nBins = 1;
    vector<vector<MCfloat> > containers;
    for (uint i = 0; i < nBins; ++i) {
        containers.push_back(vector<MCfloat>());
    }

    size_t c = 0;
    for (uint type = 0; type < 4; ++type) {
        if(dataTimes[type] == NULL)
            continue;
        c = 0;
        for (u_int64_t i = 0; i < photonCounters[type]; ++i) {
            unsigned int index = (dataTimes[type][i]-minVal)/binSize;
            containers[index].push_back(dataPoints[type][c++]);
            containers[index].push_back(dataPoints[type][c++]);
        }
        free(dataTimes[type]);
        free(dataPoints[type]);
    }

    H5Movie movie;
    movie.newFile(fileName.toAscii());

    MCfloat left = sq.left();
    MCfloat bottom = sq.bottom();
    const unsigned int nBinsX = ceil(sq.width()/binSizeX);
    const unsigned int nBinsY = nBinsX;

    hsize_t dims[3];
    dims[0] = nBinsX;
    dims[1] = nBinsY;
    dims[2] = 1;

    movie.newDataset("histogram",3,dims,dims,PredType::NATIVE_UINT64);

    u_int64_t *hist = (u_int64_t*)malloc(nBinsX*nBinsY*sizeof(u_int64_t));

    for (hsize_t i = startFrame; i < endFrame; ++i) {
        if(i >= nBins)
            break;
        memset(hist,0,nBinsX*nBinsY*sizeof(u_int64_t)); //clear hist
        c = 0;
        MCfloat *d = containers[i].data();
        size_t nCoords = containers[i].size();
        for (int n = 0; n < nCoords; n+=2) {
            size_t x = (d[n]-left) / binSizeX;
            size_t y = (d[n+1]-bottom) / binSizeX;
            if(x < nBinsX && y < nBinsY)
                hist[x*nBinsY+y]++;
        }
        movie.writeFrame(hist,i);
        containers[i].clear();
    }

    movie.close();
    containers.clear();
}

void MCMovieCreator::setTimeRange(const hsize_t startFrame, const hsize_t endFrame)
{
    this->startFrame = startFrame;
    this->endFrame = endFrame;
}

void MCMovieCreator::setBinSize(const MCfloat ps)
{
    binSize = ps;
}

void MCMovieCreator::setBinSizeX(const MCfloat um)
{
    binSizeX = um;
}

void MCMovieCreator::setSquare(const QRectF pos)
{
    sq = pos;
}

void MCMovieCreator::setWalkerFlags(unsigned int value)
{
    wFlags = value;
}

