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
#ifdef PRINT_DEBUG_MSG
    logMessage("Opening dataset XMLDescription...");
#endif
    //read whole file to string
    std::ifstream f(inputFile);
    std::string str((std::istreambuf_iterator<char>(f)),
                    std::istreambuf_iterator<char>());

    hsize_t     dims[1] = {1};


    StrType dtype(H5T_C_S1,str.size());
    StrType memtype = dtype;

    DataSpace dspace(1,dims,NULL);

    DataSet dset = file->createDataSet("XMLDescription",dtype,dspace,H5P_DEFAULT);
    dset.write(str.data(),memtype,H5S_ALL, H5S_ALL, H5P_DEFAULT);

    dset.close();
    dspace.close();
    dtype.close();
    memtype.close();
}

string H5OutputFile::loadXMLDescription()
{
#ifdef PRINT_DEBUG_MSG
    logMessage("Opening dataset XMLDescription...");
#endif
    string str;
    if(!dataSetExists("XMLDescription"))
        return str;
    size_t      sdim;

    DataSet dset = file->openDataSet("XMLDescription");
    DataType dtype = dset.getDataType();

    sdim = dtype.getSize() + 1; //including '\0'
    DataSpace dspace = dset.getSpace();

    char buf[sdim];

    StrType memtype(H5T_C_S1,sdim);

    dset.read(buf,memtype,H5S_ALL, H5S_ALL, H5P_DEFAULT);
    dset.close();
    dspace.close();
    dtype.close();
    memtype.close();

    str = buf;
    return str;
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
