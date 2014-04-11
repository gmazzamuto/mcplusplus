#include "h5outputfile.h"

#include <fstream>

H5OutputFile::H5OutputFile()
    : H5FileHelper()
{
}

bool H5OutputFile::newFile(const char *fileName)
{
    bool ret = false;
    ret = H5FileHelper::newFile(fileName);
    if(!ret)
        return false;

    ret = createDatasets();
    if(!ret)
        return false;
    return true;
}

void H5OutputFile::appendTransmittedExitPoints(const MCfloat *buffer, const hsize_t size)
{
    appendTo1Ddataset("exit-points/transmitted",buffer,size);
}

void H5OutputFile::appendReflectedExitPoints(const MCfloat *buffer, const hsize_t size)
{
    appendTo1Ddataset("exit-points/reflected",buffer,size);
}

void H5OutputFile::appendTransmittedWalkTimes(const MCfloat *buffer, const hsize_t size)
{
    appendTo1Ddataset("walk-times/transmitted",buffer,size);
}

void H5OutputFile::appendReflectedWalkTimes(const MCfloat *buffer, const hsize_t size)
{
    appendTo1Ddataset("walk-times/reflected",buffer,size);
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
#ifdef PRINT_DEBUG_MSG
    logMessage("Opening dataset %s...", datasetName);
#endif
    hsize_t     dims[1] = {1};

    const char *wdata[1];
    wdata[0] = str.data();
    StrType dtype(0, H5T_VARIABLE);

    DataSpace dspace(1,dims);

    DataSet dset = file->createDataSet(datasetName,dtype,dspace);
    dset.write((void*)wdata,dtype);

    dset.close();
    dspace.close();
    dtype.close();
}

string H5OutputFile::readVLenString(const char *datasetName)
{
#ifdef PRINT_DEBUG_MSG
    logMessage("Opening dataset %s...", datasetName);
#endif
    string str;
    if(!dataSetExists("XMLDescription"))
        return str;

    DataSet dset = file->openDataSet(datasetName);
    DataType dtype = dset.getDataType();

    DataSpace dspace = dset.getSpace();

    char *buf[1];

    dset.read(buf,dtype,H5P_DEFAULT);
    str = buf[0];

    dset.close();
    dspace.close();
    dtype.close();

    H5Dvlen_reclaim(dtype.getId(),dspace.getId(),H5P_DEFAULT,buf);

    return str;
}

void H5OutputFile::loadTransmittedExitPoints(MCfloat *destBuffer, const hsize_t *start, const hsize_t *count)
{
    loadFrom1Ddataset("exit-points/transmitted",destBuffer,start,count);
}

void H5OutputFile::loadReflectedExitPoints(MCfloat *destBuffer, const hsize_t *start, const hsize_t *count)
{
    loadFrom1Ddataset("exit-points/reflected",destBuffer,start,count);
}

void H5OutputFile::loadTransmittedWalkTimes(MCfloat *destBuffer, const hsize_t *start, const hsize_t *count)
{
    loadFrom1Ddataset("walk-times/transmitted",destBuffer,start,count);
}

void H5OutputFile::loadReflectedWalkTimes(MCfloat *destBuffer, const hsize_t *start, const hsize_t *count)
{
    loadFrom1Ddataset("walk-times/reflected",destBuffer,start,count);
}

unsigned long H5OutputFile::transmitted()
{
    openDataSet("exit-points/transmitted");
    return extentDims()[0]/2;
}

unsigned long H5OutputFile::reflected()
{
    openDataSet("exit-points/reflected");
    return extentDims()[0]/2;
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

bool H5OutputFile::createDatasets()
{

    newGroup("exit-points");

    int ndims = 1;
    hsize_t dims[ndims], chunkDims[ndims];

    dims[0] = 0; chunkDims[0] = 2;
    bool ret = false;
    ret = newDataset("exit-points/reflected",ndims,dims,chunkDims);
    if(!ret)
        return false;
    ret = newDataset("exit-points/transmitted",ndims,dims,chunkDims);
    if(!ret)
        return false;

    chunkDims[0] = 1;

    newGroup("walk-times");
    ret = newDataset("walk-times/reflected",ndims,dims,chunkDims);
    ret = newDataset("walk-times/transmitted",ndims,dims,chunkDims);

    return true;
}
