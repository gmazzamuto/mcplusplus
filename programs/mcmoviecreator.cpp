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




    MCfloat minVal=startFrame*binSize, maxVal=endFrame*binSize;

    unsigned int nBins = ceil((maxVal - minVal)/binSize);

    if(nBins == 0)
        nBins = 1;

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

    hsize_t nBinsPerFrame = nBinsX*nBinsY;

    movie.newDataset("histogram",3,dims,dims,PredType::NATIVE_UINT64);

    u_int64_t *hist = (u_int64_t*)malloc(nBins*nBinsPerFrame*sizeof(u_int64_t));


    size_t c = 0;
    for (uint type = 0; type < 4; ++type) {
        if(dataTimes[type] == NULL)
            continue;
        c = 0;
        for (u_int64_t i = 0; i < photonCounters[type]; ++i) {
            const MCfloat time = dataTimes[type][i];
            if(time > maxVal || time < minVal) {
                c++; c++;
                continue;
            }
            unsigned int t = (time-minVal)/binSize;
            size_t x = (dataPoints[type][c++]-left) / binSizeX;
            size_t y = (dataPoints[type][c++]-bottom) / binSizeX;
            if(t < nBins && x < nBinsX && y < nBinsY)
                hist[t*nBinsPerFrame+x*nBinsY+y]++;
        }
        free(dataTimes[type]);
        free(dataPoints[type]);
    }


    for (hsize_t i = 0; i < nBins; ++i) {
        movie.writeFrame(&hist[i*nBinsPerFrame],startFrame + i);
    }

    movie.close();
    free(hist);
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

