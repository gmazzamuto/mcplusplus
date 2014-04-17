#include "h5outputfile.h"
#include "xmlparser.h"

#include <fstream>
#include <string.h>
#include <iostream>

H5OutputFile::H5OutputFile()
    : H5FileHelper()
{
    _transmitted = 0;
    _ballistic = 0;
    _reflected = 0;
    _backReflected = 0;
    _parser = new XMLParser();
    XMLParserEnabled = false;
}

H5OutputFile::~H5OutputFile()
{
    delete _parser;
}

bool H5OutputFile::newFile(const char *fileName, bool create_datasets)
{
    bool ret = false;
    ret = H5FileHelper::newFile(fileName);
    if(!ret)
        return false;

    if(create_datasets) {
        ret = createDatasets(Simulation::SAVE_ALL_WALKERS, Simulation::SAVE_ALL_WALKERS, Simulation::SAVE_ALL_WALKERS);
        if(!ret)
            return false;
    }
    return true;
}

bool H5OutputFile::newFromXML(const char *xmlFile, const char *fileName)
{
    bool ret = newFile(fileName,false);
    if(!ret)
        return false;
    XMLParser parser;
    parser.setXMLFile(xmlFile);
    parser.parseOutput();

    createDatasets(parser.walkTimesSaveFlags(),parser.exitPointsSaveFlags(),parser.exitKVectorsSaveFlags());

    writeXMLDescription(xmlFile);
    return true;
}

void H5OutputFile::appendExitKVectors(Simulation::walkerType idx, const MCfloat *buffer, const hsize_t size)
{
    switch (idx) {
    case Simulation::TRANSMITTED:
        appendTo1Ddataset("exit-k-vectors/transmitted",buffer,size);
        break;
    case Simulation::BALLISTIC:
        appendTo1Ddataset("exit-k-vectors/ballistic",buffer,size);
        break;
    case Simulation::REFLECTED:
        appendTo1Ddataset("exit-k-vectors/reflected",buffer,size);
        break;
    case Simulation::BACKREFLECTED:
        appendTo1Ddataset("exit-k-vectors/back-reflected",buffer,size);
        break;
    default:
        break;
    }
}

void H5OutputFile::appendExitPoints(Simulation::walkerType idx, const MCfloat *buffer, const hsize_t size)
{
    switch (idx) {
    case Simulation::TRANSMITTED:
        appendTo1Ddataset("exit-points/transmitted",buffer,size);
        break;
    case Simulation::BALLISTIC:
        appendTo1Ddataset("exit-points/ballistic",buffer,size);
        break;
    case Simulation::REFLECTED:
        appendTo1Ddataset("exit-points/reflected",buffer,size);
        break;
    case Simulation::BACKREFLECTED:
        appendTo1Ddataset("exit-points/back-reflected",buffer,size);
        break;
    default:
        break;
    }
}

void H5OutputFile::appendWalkTimes(Simulation::walkerType idx, const MCfloat *buffer, const hsize_t size)
{
    switch (idx) {
    case Simulation::TRANSMITTED:
        appendTo1Ddataset("walk-times/transmitted",buffer,size);
        break;
    case Simulation::BALLISTIC:
        appendTo1Ddataset("walk-times/ballistic",buffer,size);
        break;
    case Simulation::REFLECTED:
        appendTo1Ddataset("walk-times/reflected",buffer,size);
        break;
    case Simulation::BACKREFLECTED:
        appendTo1Ddataset("walk-times/back-reflected",buffer,size);
        break;
    default:
        break;
    }
}

void H5OutputFile::loadExitPoints(Simulation::walkerType idx, MCfloat *destBuffer, const hsize_t *start, const hsize_t *count)
{
    switch (idx) {
    case Simulation::TRANSMITTED:
        loadFrom1Ddataset("exit-points/transmitted",destBuffer,start,count);
        break;
    case Simulation::BALLISTIC:
        loadFrom1Ddataset("exit-points/ballistic",destBuffer,start,count);
        break;
    case Simulation::REFLECTED:
        loadFrom1Ddataset("exit-points/reflected",destBuffer,start,count);
        break;
    case Simulation::BACKREFLECTED:
        loadFrom1Ddataset("exit-points/back-reflected",destBuffer,start,count);
        break;
    default:
        break;
    }
}

void H5OutputFile::loadWalkTimes(Simulation::walkerType idx, MCfloat *destBuffer, const hsize_t *start, const hsize_t *count)
{
    switch (idx) {
    case Simulation::TRANSMITTED:
        loadFrom1Ddataset("walk-times/transmitted",destBuffer,start,count);
        break;
    case Simulation::BALLISTIC:
        loadFrom1Ddataset("walk-times/ballistic",destBuffer,start,count);
        break;
    case Simulation::REFLECTED:
        loadFrom1Ddataset("walk-times/reflected",destBuffer,start,count);
        break;
    case Simulation::BACKREFLECTED:
        loadFrom1Ddataset("walk-times/back-reflected",destBuffer,start,count);
        break;
    default:
        break;
    }
}

void H5OutputFile::loadExitKVectors(Simulation::walkerType idx, MCfloat *destBuffer, const hsize_t *start, const hsize_t *count)
{
    switch (idx) {
    case Simulation::TRANSMITTED:
        loadFrom1Ddataset("exit-k-vectors/transmitted",destBuffer,start,count);
        break;
    case Simulation::BALLISTIC:
        loadFrom1Ddataset("exit-k-vectors/ballistic",destBuffer,start,count);
        break;
    case Simulation::REFLECTED:
        loadFrom1Ddataset("exit-k-vectors/reflected",destBuffer,start,count);
        break;
    case Simulation::BACKREFLECTED:
        loadFrom1Ddataset("exit-k-vectors/back-reflected",destBuffer,start,count);
        break;
    default:
        break;
    }
}

void H5OutputFile::appendTo1Ddataset(const char *datasetName, const MCfloat *buffer, const hsize_t size) {
    if(!size)
        return;
    openDataSet(datasetName);

    hsize_t dims[1];
    *dims = *extentDims();

    writeHyperSlab(dims,&size,buffer);
}

void H5OutputFile::loadFrom1Ddataset(const char *datasetName, MCfloat *destBuffer, const hsize_t *start, const hsize_t *count)
{
    openDataSet(datasetName);
    if(count!=NULL)
        loadHyperSlab(start,count,destBuffer);
    else
        loadAll(destBuffer);
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
    if(XMLParserEnabled) {
        vector<string> RNGStates;
        _parser->parseString(readXMLDescription());
        uint s = 0;
        while (1) {
            stringstream ss;
            ss << "RNGStates/seed";
            ss << s;
            if(!dataSetExists(ss.str().c_str()))
                break;
            RNGStates.push_back(readRNGState(s));
            s++;
        }
        _parser->simulation()->setMultipleRNGStates(RNGStates);
    }

    DataSet dSet = file->openDataSet("photon-counters");

    u_int64_t buf[4];
    dSet.read(buf,dSet.getDataType());
    _transmitted = buf[0];
    _ballistic = buf[1];
    _reflected = buf[2];
    _backReflected = buf[3];

    dSet.close();
    return true;
}

void H5OutputFile::saveRNGState(const uint seed, const string s)
{
    stringstream ss;
    ss << "RNGStates/seed";
    ss << seed;
    createRNGDataset(seed);
    writeVLenString(ss.str().c_str(),s);
}

string H5OutputFile::readRNGState(const uint seed) const
{
    stringstream ss;
    ss << "RNGStates/seed";
    ss << seed;
    return readVLenString(ss.str().c_str());
}

void H5OutputFile::appendPhotonCounts(const u_int64_t transmitted, const u_int64_t ballistic, const u_int64_t reflected, const u_int64_t backReflected)
{
    _transmitted += transmitted;
    _ballistic += ballistic;
    _reflected += reflected;
    _backReflected += backReflected;

    u_int64_t buf[4];
    buf[0] = _transmitted;
    buf[1] = _ballistic;
    buf[2] = _reflected;
    buf[3] = _backReflected;
    DataSet dset = file->openDataSet("photon-counters");
    dset.write(buf,dset.getDataType());
    dset.close();
}

u_int64_t H5OutputFile::transmitted()
{
    return _transmitted;
}

u_int64_t H5OutputFile::ballistic()
{
    return _ballistic;
}

u_int64_t H5OutputFile::reflected()
{
    return _reflected;
}

u_int64_t H5OutputFile::backReflected()
{
    return _backReflected;
}

void H5OutputFile::writeXMLDescription(const char *inputFile)
{
    //read whole file to string
    std::ifstream f(inputFile);
    std::string str((std::istreambuf_iterator<char>(f)),
                    std::istreambuf_iterator<char>());

    writeVLenString("XMLDescription", str);
}

string H5OutputFile::readXMLDescription()
{
    return readVLenString("XMLDescription");
}

Simulation *H5OutputFile::simulation() const
{
    return _parser->simulation();
}

XMLParser *H5OutputFile::xmlParser() const
{
    return _parser;
}

void H5OutputFile::setXMLParserEnabled(bool enable)
{
    XMLParserEnabled = enable;
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

    newGroup("RNGStates");

    if(exitPointsSaveFlags) {
        newGroup("exit-points");

        dims[0] = 0; chunkDims[0] = 2*NWALKER_CHUNK;
        if(exitPointsSaveFlags & Simulation::SAVE_TRANSMITTED)
            ret = newDataset("exit-points/transmitted",ndims,dims,chunkDims);
        if(exitPointsSaveFlags & Simulation::SAVE_BALLISTIC)
            ret = newDataset("exit-points/ballistic",ndims,dims,chunkDims);
        if(exitPointsSaveFlags & Simulation::SAVE_REFLECTED)
            ret = newDataset("exit-points/reflected",ndims,dims,chunkDims);
        if(exitPointsSaveFlags & Simulation::SAVE_BACKREFLECTED)
            ret = newDataset("exit-points/back-reflected",ndims,dims,chunkDims);
    }

    if(walkTimesSaveFlags) {
        dims[0] = 0; chunkDims[0] = NWALKER_CHUNK;
        newGroup("walk-times");
        if(walkTimesSaveFlags & Simulation::SAVE_TRANSMITTED)
            ret = newDataset("walk-times/transmitted",ndims,dims,chunkDims);
        if(walkTimesSaveFlags & Simulation::SAVE_BALLISTIC)
            ret = newDataset("walk-times/ballistic",ndims,dims,chunkDims);
        if(walkTimesSaveFlags & Simulation::SAVE_REFLECTED)
            ret = newDataset("walk-times/reflected",ndims,dims,chunkDims);
        if(walkTimesSaveFlags & Simulation::SAVE_BACKREFLECTED)
            ret = newDataset("walk-times/back-reflected",ndims,dims,chunkDims);
    }

    if(exitKVectorsSaveFlags) {
        dims[0] = 0; chunkDims[0] = 3*NWALKER_CHUNK/2;
        newGroup("exit-k-vectors");
        if(exitKVectorsSaveFlags & Simulation::SAVE_TRANSMITTED)
            ret = newDataset("exit-k-vectors/transmitted",ndims,dims,chunkDims);
        if(exitKVectorsSaveFlags & Simulation::SAVE_BALLISTIC)
            ret = newDataset("exit-k-vectors/ballistic",ndims,dims,chunkDims);
        if(exitKVectorsSaveFlags & Simulation::SAVE_REFLECTED)
            ret = newDataset("exit-k-vectors/reflected",ndims,dims,chunkDims);
        if(exitKVectorsSaveFlags & Simulation::SAVE_BACKREFLECTED)
            ret = newDataset("exit-k-vectors/back-reflected",ndims,dims,chunkDims);
    }

    dims[0] = 1;
    StrType dtype(0, H5T_VARIABLE);
    DataSpace dspace(1,dims,dims);
    file->createDataSet("XMLDescription",dtype,dspace);
    dtype.close();
    dspace.close();

    return ret;
}

bool H5OutputFile::createRNGDataset(uint seed)
{
    int ndims = 1;
    hsize_t dims[ndims];
    dims[0] = 1;
    StrType dtype(0, H5T_VARIABLE);
    DataSpace dspace(1,dims,dims);
    stringstream ss;
    ss << "RNGStates/seed";
    ss << seed;
    if(!dataSetExists(ss.str().c_str())) {
#ifdef PRINT_DEBUG_MSG
        logMessage("Creating dataset %s",ss.str().c_str());
#endif
        file->createDataSet(ss.str(),dtype,dspace);
    }
    dtype.close();
    dspace.close();
    return true;
}
