#include "histogram.h"
#include <cmath>

#include "h5filehelper.h"

#include <boost/math/constants/constants.hpp>
#include <sstream>


using namespace boost::math::constants;

using namespace MCPP;

Histogram::Histogram(BaseObject *parent) :
    BaseObject(parent)
{
    type[0] = DATA_NONE;
    type[1] = DATA_NONE;
    min[0] = 0;
    max[0] = 0;
    min[1] = 0;
    max[1] = 0;
    binSize[0] = 0;
    binSize[1] = 0;
    histo = NULL;
    variance = NULL;
    computeSpatialVariance = false;
    photonTypeFlags = -1;
    scale = 1;
    histName = "";
}

Histogram::~Histogram()
{
    if(histo != NULL)
        free(histo);
    if(variance != NULL)
        free(variance);
}

void Histogram::setType(const MCData type1, const MCData type2)
{
    type[0] = type1;
    type[1] = type2;
}

void Histogram::setMax(const double max1, const double max2)
{
    max[0] = max1;
    max[1] = max2;
}

void Histogram::setMax2(const double max2)
{
    max[1] = max2;
}

void Histogram::setMin(const double min1, const double min2)
{
    min[0] = min1;
    min[1] = min2;
}

void Histogram::setMin2(const double min2)
{
    min[1] = min2;
}

void Histogram::setBinSize(const double binSize1, const double binSize2)
{
    binSize[0] = binSize1;
    binSize[1] = binSize2;
}

void Histogram::setBinSize2(const double binSize2)
{
    binSize[1] = binSize2;
}

void Histogram::setSpatialVarianceEnabled(const bool enable)
{
    computeSpatialVariance = enable;
}

bool Histogram::is1D() const
{
    return (type[1] == DATA_NONE);
}

bool Histogram::is2D() const
{
    return !is1D();
}

bool Histogram::initialize()
{
    if(!sanityCheck())
        return false;
    degPerRad = 180/pi<MCfloat>();
    nBins[0] = ceil((max[0] - min[0]) / binSize[0]) + 1; //+1 for overflow bin
    if(is2D())
        nBins[1] = ceil((max[1] - min[1]) / binSize[1]) + 1;
    else
        nBins[1] = 1;
    totBins = nBins[0]*nBins[1];
    histo = (u_int64_t *)calloc(totBins,sizeof(u_int64_t));
    if(computeSpatialVariance)
        variance = (MCfloat *)calloc(totBins,sizeof(MCfloat));
    else
        variance = NULL;

    firstBinEdge[0] = min[0];
    firstBinEdge[1] = min[1];
    firstBinCenter[0] = firstBinEdge[0] + binSize[0]*0.5;
    firstBinCenter[1] = firstBinEdge[1] + binSize[1]*0.5;
    return true;
}

void Histogram::setPhotonTypeFlags(int value)
{
    photonTypeFlags = value;
}

void Histogram::run(const Walker * const buf, size_t bufSize)
{
    for (uint i = 0; i < bufSize; ++i) {
        const Walker * const w = &buf[i];
        if(!pickPhoton(w))
            continue;

        size_t index[2] = {0,0};
        switch(type[0]) {
        case DATA_K:
            index[0] = (acos(w->k0[2])*degPerRad - firstBinEdge[0])/binSize[0];
            break;

        case DATA_POINTS: {
            MCfloat module = sqrt(pow(w->r0[0], 2) + pow(w->r0[1], 2));
            index[0] = (module - firstBinEdge[0])/binSize[0];
        }
            break;

        case DATA_TIMES:
            index[0] = (w->walkTime - firstBinEdge[0])/binSize[0];
            break;
        }

        if(index[0] > nBins[0] -1)
            index[0] = nBins[0] -1;

        if(is2D()) {
            switch(type[1]) {
            case DATA_K:
                index[1] = (acos(w->k0[2])*degPerRad - firstBinEdge[1])/binSize[1];
                break;

            case DATA_POINTS: {
                MCfloat module = sqrt(pow(w->r0[0], 2) + pow(w->r0[1], 2));
                index[1] = (module - firstBinEdge[1])/binSize[1];
            }
                break;

            case DATA_TIMES:
                index[1] = (w->walkTime - firstBinEdge[1])/binSize[1];
                break;
            }

            if(index[1] > nBins[1] - 1)
                index[1] = nBins[1] - 1;
        }
        size_t idx =  index[0]*nBins[1] + index[1];
        histo[idx]++;
        if(computeSpatialVariance) {
            MCfloat module2 = pow(w->r0[0], 2) + pow(w->r0[1], 2);
            variance[idx] += module2;
        }
    }
}

bool Histogram::pickPhoton(const Walker * const w)
{
    walkerFlags flag = walkerTypeToFlag(w->type);
    if(!(photonTypeFlags & flag))
        return false;
    return pickPhoton_impl(w);
}

void Histogram::appendCounts(const Histogram *rhs)
{
    for (size_t i = 0; i < totBins; ++i) {
        histo[i] += rhs->histo[i];
    }
    if(variance != NULL && rhs->variance!= NULL)
        for (size_t i = 0; i < totBins; ++i) {
            variance[i] += rhs->variance[i];
    }
}

void Histogram::dump() const
{
    u_int64_t total = 0;
    for (size_t i = 0; i < nBins[0]; ++i) {
        for (size_t j = 0; j < nBins[1]; ++j) {
            size_t idx = i*nBins[1] + j;
            cout << histo[idx] << "\t";
            total += histo[idx];
        }
        cout << endl;
    }
    cout << "total: " << total << endl;
}

void Histogram::saveToFile(const char *fileName) const
{
    string _dsName = histName;
    if(_dsName == "")
        _dsName = "histogram";
    H5FileHelper *file = new H5FileHelper(0);
    if(access(fileName,F_OK)<0)
        file->newFile(fileName);
    else
        file->openFile(fileName);
    hsize_t dims[2] = {nBins[0],nBins[1]+1};
    if(computeSpatialVariance)
        dims[1]++;
    file->newDataset(_dsName.c_str(),2,dims);

    uint ncols = dims[1];
    string colNames[ncols];

    MCfloat *data;
    data = (MCfloat *)malloc(totBins*sizeof(MCfloat));

    hsize_t start[2];
    hsize_t count[2];

    for (size_t i = 0; i < nBins[0]; ++i) {
        data[i] = 1.* firstBinCenter[0] + i*binSize[0];
    }

    start[0] = 0;
    start[1] = 0;
    count[0] = nBins[0];
    count[1] = 1;

    file->writeHyperSlab(start, count, data);



    switch (type[0]) {
    case DATA_K:
        colNames[0] = "k";
        for (size_t i = 0; i < nBins[0]; ++i) {
            MCfloat scale2 = scale*4.0*pi<MCfloat>()*sin((i+0.5)*binSize[0]/degPerRad)*sin(binSize[0]/2./degPerRad);
            for (size_t j = 0; j < nBins[1]; ++j)
                data[i*nBins[1] + j] = 1. * histo[i*nBins[1] + j] / scale2;
        }
        break;

    case DATA_TIMES:
        colNames[0] = "time";
        for (size_t i = 0; i < nBins[0]; ++i) {
            MCfloat scale2 = scale;
            for (size_t j = 0; j < nBins[1]; ++j)
                data[i*nBins[1] + j] = 1. * histo[i*nBins[1] + j] / scale2;
        }
        break;

    case DATA_POINTS:
        colNames[0] = "um";
        for (size_t i = 0; i < nBins[0]; ++i) {
            MCfloat dr = binSize[0];
            MCfloat scale2 = scale * (2*pi<MCfloat>()*(i+0.5)*dr*dr);
            for (size_t j = 0; j < nBins[1]; ++j)
               data[i*nBins[1] + j] = 1. * histo[i*nBins[1] + j] / scale2;
        }
        break;

    default:
        break;
    }

    start[0] = 0;
    start[1] = 1;
    count[0] = nBins[0];
    count[1] = nBins[1];

    file->writeHyperSlab(start, count, data);



    if(computeSpatialVariance) {
        colNames[2] = "variance";

        for (size_t i = 0; i < nBins[0]; ++i)
                data[i] = variance[i] / histo[i];

        start[0] = 0;
        start[1] = 2;
        count[0] = nBins[0];
        count[1] = 1;

        file->writeHyperSlab(start, count, data);
    }

    if(is1D())
        colNames[1] = "normCounts";
    else {
        for (size_t j = 0; j < nBins[1]; ++j) {
            stringstream ss;
            ss << "b-" << firstBinCenter[1] + j*binSize[1];
            colNames[j+1] = ss.str();
        }
    }

    file->writeColumnNames(ncols,colNames);


    free(data);
    delete file;
}

void Histogram::setScale(u_int64_t totalPhotons)
{
    scale = totalPhotons;
}

void Histogram::setName(const char *name)
{
    histName = name;
}

bool Histogram::pickPhoton_impl(const Walker * const w)
{
    return true;
}

BaseObject *Histogram::clone_impl() const
{
    Histogram *h = new Histogram(NULL);
    h->type[0] = type[0];
    h->type[1] = type[1];
    h->min[0] = min[0];
    h->min[1] = min[1];
    h->max[0] = max[0];
    h->max[1] = max[1];
    h->binSize[0] = binSize[0];
    h->binSize[1] = binSize[1];
    h->computeSpatialVariance = computeSpatialVariance;
    h->photonTypeFlags = photonTypeFlags;
    h->scale = scale;
    h->initialize();
    return h;
}

bool Histogram::sanityCheck_impl() const
{
    if(photonTypeFlags < 0)
        return false;
    if(type[0] == DATA_NONE)
        return false;
    if(binSize[0] == 0)
        return false;
    if(is2D() && binSize[1] == 0)
        return false;
    if(max[0] - min[0] <= 0)
        return false;
    if(is2D() && max[1] - min[1] <= 0)
        return false;
    if(computeSpatialVariance) {
        if(is2D())
            return false;
        if(type[0] != DATA_TIMES)
            return false;
    }
    return true;
}
