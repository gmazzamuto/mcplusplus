#include "h5outputfile.h"

#include <fstream>
#include <string.h>

#define MAX_RNGSTATE_STR_LEN 6900

using namespace MCPP;

H5OutputFile::H5OutputFile()
    : H5FileHelper()
{
    memset(_photonCounters,0,4*sizeof(u_int64_t));
}

H5OutputFile::~H5OutputFile()
{
}

bool H5OutputFile::newFile(const char *fileName, bool create_datasets)
{
    bool ret = false;
    ret = H5FileHelper::newFile(fileName);
    if(!ret)
        return false;

    if(create_datasets) {
        ret = createDatasets(FLAG_ALL_WALKERS, FLAG_ALL_WALKERS, FLAG_ALL_WALKERS);
        if(!ret)
            return false;
    }
    return true;
}

void H5OutputFile::appendExitKVectors(walkerType type, const MCfloat *buffer, const hsize_t size)
{
    switch (type) {
    case TRANSMITTED:
        appendTo1Ddataset("exit-k-vectors/transmitted",buffer,size);
        break;
    case BALLISTIC:
        appendTo1Ddataset("exit-k-vectors/ballistic",buffer,size);
        break;
    case REFLECTED:
        appendTo1Ddataset("exit-k-vectors/reflected",buffer,size);
        break;
    case BACKREFLECTED:
        appendTo1Ddataset("exit-k-vectors/back-reflected",buffer,size);
        break;
    default:
        break;
    }
}

void H5OutputFile::appendExitPoints(walkerType type, const MCfloat *buffer, const hsize_t size)
{
    switch (type) {
    case TRANSMITTED:
        appendTo1Ddataset("exit-points/transmitted",buffer,size);
        break;
    case BALLISTIC:
        appendTo1Ddataset("exit-points/ballistic",buffer,size);
        break;
    case REFLECTED:
        appendTo1Ddataset("exit-points/reflected",buffer,size);
        break;
    case BACKREFLECTED:
        appendTo1Ddataset("exit-points/back-reflected",buffer,size);
        break;
    default:
        break;
    }
}

void H5OutputFile::appendWalkTimes(walkerType type, const MCfloat *buffer, const hsize_t size)
{
    switch (type) {
    case TRANSMITTED:
        appendTo1Ddataset("walk-times/transmitted",buffer,size);
        break;
    case BALLISTIC:
        appendTo1Ddataset("walk-times/ballistic",buffer,size);
        break;
    case REFLECTED:
        appendTo1Ddataset("walk-times/reflected",buffer,size);
        break;
    case BACKREFLECTED:
        appendTo1Ddataset("walk-times/back-reflected",buffer,size);
        break;
    default:
        break;
    }
}

bool H5OutputFile::loadExitPoints(walkerType type, MCfloat *destBuffer, const hsize_t *start, const hsize_t *count)
{
    bool ret = false;
    switch (type) {
    case TRANSMITTED:
        ret = loadFrom1Ddataset("exit-points/transmitted",destBuffer,start,count);
        break;
    case BALLISTIC:
        ret = loadFrom1Ddataset("exit-points/ballistic",destBuffer,start,count);
        break;
    case REFLECTED:
        ret = loadFrom1Ddataset("exit-points/reflected",destBuffer,start,count);
        break;
    case BACKREFLECTED:
        ret = loadFrom1Ddataset("exit-points/back-reflected",destBuffer,start,count);
        break;
    default:
        break;
    }
    return ret;
}

bool H5OutputFile::loadWalkTimes(walkerType type, MCfloat *destBuffer, const hsize_t *start, const hsize_t *count)
{
    bool ret = false;
    switch (type) {
    case TRANSMITTED:
        ret = loadFrom1Ddataset("walk-times/transmitted",destBuffer,start,count);
        break;
    case BALLISTIC:
        ret = loadFrom1Ddataset("walk-times/ballistic",destBuffer,start,count);
        break;
    case REFLECTED:
        ret = loadFrom1Ddataset("walk-times/reflected",destBuffer,start,count);
        break;
    case BACKREFLECTED:
        ret = loadFrom1Ddataset("walk-times/back-reflected",destBuffer,start,count);
        break;
    default:
        break;
    }
    return ret;
}

bool H5OutputFile::loadExitKVectors(walkerType type, MCfloat *destBuffer, const hsize_t *start, const hsize_t *count)
{
    bool ret = false;
    switch (type) {
    case TRANSMITTED:
        ret = loadFrom1Ddataset("exit-k-vectors/transmitted",destBuffer,start,count);
        break;
    case BALLISTIC:
        ret = loadFrom1Ddataset("exit-k-vectors/ballistic",destBuffer,start,count);
        break;
    case REFLECTED:
        ret = loadFrom1Ddataset("exit-k-vectors/reflected",destBuffer,start,count);
        break;
    case BACKREFLECTED:
        ret = loadFrom1Ddataset("exit-k-vectors/back-reflected",destBuffer,start,count);
        break;
    default:
        break;
    }
    return ret;
}

bool H5OutputFile::loadData(MCData group, walkerType type, MCfloat *destBuffer, const hsize_t *start, const hsize_t *count)
{
    stringstream ss;
    switch (group) {
    case DATA_POINTS:
        ss << "exit-points";
        break;
    case DATA_K:
        ss << "exit-k-vectors";
        break;
    case DATA_TIMES:
        ss << "walk-times";
        break;
    default:
        break;
    }

    ss << "/" << walkerTypeToString(type);
    return loadFrom1Ddataset(ss.str().c_str(),destBuffer,start,count);
}

void H5OutputFile::appendTo1Ddataset(const char *datasetName, const MCfloat *buffer, const hsize_t size) {
    if(!size)
        return;
    openDataSet(datasetName);

    hsize_t dims[1];
    *dims = *extentDims();

    writeHyperSlab(dims,&size,buffer);
}

bool H5OutputFile::loadFrom1Ddataset(const char *datasetName, MCfloat *destBuffer, const hsize_t *start, const hsize_t *count)
{
    if(!openDataSet(datasetName))
        return false;
    if(count!=NULL)
        loadHyperSlab(start,count,destBuffer);
    else
        loadAll(destBuffer);
    return true;
}

void H5OutputFile::writeVLenString(const char *datasetName, const string str)
{
    closeDataSet();
#ifdef PRINT_DEBUG_MSG
    logMessage("Opening dataset %s...", datasetName);
#endif

    const char *wdata[1];
    wdata[0] = str.data();

    DataSet dset = file->openDataSet(datasetName);
    DataType dtype = dset.getDataType();
    dset.write((void*)wdata,dtype);

    dset.close();
}

void H5OutputFile::saveRNGState(uint seed, const string str) {
    openDataSet("RNGStates");
    if(seed > dims[0] - 1) {
        hsize_t extDims[ndims];
        extDims[0] = seed+1;

        dataSet->extend(extDims);

        *dataSpace = dataSet->getSpace();
        dataSpace->getSimpleExtentDims(dims);
    }

    hsize_t offset[ndims], mdims[ndims];
    offset[0] = seed;
    mdims[0] = 1;
    DataType dtype = dataSet->getDataType();

    hsize_t _offset[ndims];
    for (int i = 0; i < ndims; ++i)
        _offset[i] = 0;

    DataSpace memspace( ndims, mdims );
    memspace.selectHyperslab(H5S_SELECT_SET, mdims, _offset);
    dataSpace->selectHyperslab(H5S_SELECT_SET, mdims, offset);
    dataSet->write(str.c_str(),dtype,memspace,*dataSpace);
    memspace.close();

    closeDataSet();
}

string H5OutputFile::readVLenString(const char *datasetName) const
{
#ifdef PRINT_DEBUG_MSG
    logMessage("Opening dataset %s...", datasetName);
#endif
    string str;
    if(!dataSetExists(datasetName))
        return str;

    DataSet dset = file->openDataSet(datasetName);
    DataType dtype = dset.getDataType();

    DataSpace dspace = dset.getSpace();

    char *buf[1];

    dset.read(buf,dtype);
    if(buf[0] != NULL)
        str = buf[0];

    if(buf[0] != NULL)
        H5Dvlen_reclaim(dtype.getId(),dspace.getId(),H5P_DEFAULT,buf);

    dset.close();
    dspace.close();
    dtype.close();

    return str;
}

bool H5OutputFile::openFile_impl()
{
    DataSet dSet = file->openDataSet("photon-counters");
    dSet.read(_photonCounters,dSet.getDataType());
    dSet.close();

    return true;
}

string H5OutputFile::readRNGState(const uint seed) const
{
    DataSet dset = file->openDataSet("RNGStates");
    DataType dtype = dset.getDataType();

    DataSpace dspace = dset.getSpace();

    int ndims = dspace.getSimpleExtentNdims();

    hsize_t offset[ndims], mdims[ndims];
    offset[0] = seed;
    mdims[0] = 1;

    hsize_t _offset[ndims];
    for (int i = 0; i < ndims; ++i)
        _offset[i] = 0;

    char buf[MAX_RNGSTATE_STR_LEN];

    DataSpace memspace( ndims, mdims );
    memspace.selectHyperslab(H5S_SELECT_SET, mdims, _offset);
    dspace.selectHyperslab(H5S_SELECT_SET, mdims, offset);
    dset.read(buf,dtype,memspace,dspace);

    dspace.close();
    dtype.close();
    dset.close();

    stringstream ss;
    ss << buf;
    return ss.str();
}

void H5OutputFile::appendPhotonCounts(const u_int64_t transmitted, const u_int64_t ballistic, const u_int64_t reflected, const u_int64_t backReflected)
{
    _photonCounters[TRANSMITTED] += transmitted;
    _photonCounters[BALLISTIC] += ballistic;
    _photonCounters[REFLECTED] += reflected;
    _photonCounters[BACKREFLECTED] += backReflected;

    DataSet dset = file->openDataSet("photon-counters");
    dset.write(_photonCounters,dset.getDataType());
    dset.close();
}

void H5OutputFile::appendPhotonCounts(const u_int64_t *counters)
{
    for (uint i = 0; i < 4; ++i) {
        _photonCounters[i] += counters[i];
    }
    DataSet dset = file->openDataSet("photon-counters");
    dset.write(_photonCounters,dset.getDataType());
    dset.close();
}

u_int64_t H5OutputFile::transmitted() const
{
    return _photonCounters[TRANSMITTED];
}

u_int64_t H5OutputFile::ballistic() const
{
    return _photonCounters[BALLISTIC];
}

u_int64_t H5OutputFile::reflected() const
{
    return _photonCounters[REFLECTED];
}

u_int64_t H5OutputFile::backReflected() const
{
    return _photonCounters[BACKREFLECTED];
}

const u_int64_t *H5OutputFile::photonCounters() const
{
    return _photonCounters;
}

void H5OutputFile::saveSample(const Sample *sample)
{
    char datasetName[] = "sample";
    if(dataSetExists(datasetName))
        return;

    int ndims = 2;
    hsize_t dims[ndims], chunkDims[ndims];

    hsize_t nLayers = sample->nLayers();

    dims[1] = 4; chunkDims[1] = 4;
    dims[0] = nLayers+2; chunkDims[0] = 1;
    newDataset(datasetName,ndims,dims,chunkDims);

    openDataSet(datasetName);
    dims[1] = nLayers;
    MCfloat buffer[(nLayers+2)*4];
    const deque< MCfloat > *boundaries = sample->zBoundaries();

    uint i = 0;
    buffer[i++] = -1;
    Material *mat = sample->material(0);
    buffer[i++] = mat->n;
    buffer[i++] = mat->ls;
    buffer[i++] = mat->g;

    for (uint n = 0; n < nLayers; ++n) {
        buffer[i++] = boundaries->at(n+1) - boundaries->at(n);
        Material *mat = sample->material(n+1);
        buffer[i++] = mat->n;
        buffer[i++] = mat->ls;
        buffer[i++] = mat->g;
    }

    buffer[i++] = -1;
    mat = sample->material(nLayers+1);
    buffer[i++] = mat->n;
    buffer[i++] = mat->ls;
    buffer[i++] = mat->g;

    hsize_t start[2], count[2];
    start[0] = 0; start[1] = 0;
    count[0] = nLayers+2; count[1] = 4;

    writeHyperSlab(start, count, buffer);

}

bool H5OutputFile::createDatasets(uint walkTimesSaveFlags, uint exitPointsSaveFlags, uint exitKVectorsSaveFlags)
{
#define NWALKER_CHUNK sizeof(MCfloat) * 8192
    int ndims = 1;
    hsize_t dims[ndims], chunkDims[ndims];
    bool ret = false;
    dims[0] = 4;
    try {
        DataSpace dspace(ndims, dims, dims);
        DataSet  dset = file->createDataSet("photon-counters", PredType::NATIVE_INT64, dspace);
        dspace.close();
        dset.close();
    }
    catch (Exception error) {
        logMessage("Cannot create dataset %s.\n", "photon-counters");
        return false;
    }

    createRNGDataset();

    if(exitPointsSaveFlags) {
        newGroup("exit-points");

        dims[0] = 0; chunkDims[0] = 2*NWALKER_CHUNK;
        if(exitPointsSaveFlags & FLAG_TRANSMITTED)
            ret = newDataset("exit-points/transmitted",ndims,dims,chunkDims,MCH5FLOAT);
        if(exitPointsSaveFlags & FLAG_BALLISTIC)
            ret = newDataset("exit-points/ballistic",ndims,dims,chunkDims,MCH5FLOAT);
        if(exitPointsSaveFlags & FLAG_REFLECTED)
            ret = newDataset("exit-points/reflected",ndims,dims,chunkDims,MCH5FLOAT);
        if(exitPointsSaveFlags & FLAG_BACKREFLECTED)
            ret = newDataset("exit-points/back-reflected",ndims,dims,chunkDims,MCH5FLOAT);
    }

    if(walkTimesSaveFlags) {
        dims[0] = 0; chunkDims[0] = NWALKER_CHUNK;
        newGroup("walk-times");
        if(walkTimesSaveFlags & FLAG_TRANSMITTED)
            ret = newDataset("walk-times/transmitted",ndims,dims,chunkDims,MCH5FLOAT);
        if(walkTimesSaveFlags & FLAG_BALLISTIC)
            ret = newDataset("walk-times/ballistic",ndims,dims,chunkDims,MCH5FLOAT);
        if(walkTimesSaveFlags & FLAG_REFLECTED)
            ret = newDataset("walk-times/reflected",ndims,dims,chunkDims,MCH5FLOAT);
        if(walkTimesSaveFlags & FLAG_BACKREFLECTED)
            ret = newDataset("walk-times/back-reflected",ndims,dims,chunkDims,MCH5FLOAT);
    }

    if(exitKVectorsSaveFlags) {
        dims[0] = 0; chunkDims[0] = 3*NWALKER_CHUNK/2;
        newGroup("exit-k-vectors");
        if(exitKVectorsSaveFlags & FLAG_TRANSMITTED)
            ret = newDataset("exit-k-vectors/transmitted",ndims,dims,chunkDims,MCH5FLOAT);
        if(exitKVectorsSaveFlags & FLAG_BALLISTIC)
            ret = newDataset("exit-k-vectors/ballistic",ndims,dims,chunkDims,MCH5FLOAT);
        if(exitKVectorsSaveFlags & FLAG_REFLECTED)
            ret = newDataset("exit-k-vectors/reflected",ndims,dims,chunkDims,MCH5FLOAT);
        if(exitKVectorsSaveFlags & FLAG_BACKREFLECTED)
            ret = newDataset("exit-k-vectors/back-reflected",ndims,dims,chunkDims,MCH5FLOAT);
    }

    return ret;
}

bool H5OutputFile::createRNGDataset()
{
    int ndims = 1;
    hsize_t dims[ndims], maxdims[ndims], chkdims[ndims];
    dims[0] = 1;
    maxdims[0] = H5S_UNLIMITED;
    chkdims[0] = 1000;
    StrType dtype(H5T_C_S1,MAX_RNGSTATE_STR_LEN);
    size_t size = MAX_RNGSTATE_STR_LEN;
    dtype.setSize(size);
    DataSpace dspace(1,dims,maxdims);
    stringstream ss;
    ss << "RNGStates";
    if(!dataSetExists(ss.str().c_str())) {
#ifdef PRINT_DEBUG_MSG
        logMessage("Creating dataset %s",ss.str().c_str());
#endif
        DSetCreatPropList cparms;
        cparms.setChunk(ndims,chkdims);
        cparms.setDeflate(9);
        file->createDataSet(ss.str(),dtype,dspace,cparms);
    }
    dtype.close();
    dspace.close();

    return true;
}
