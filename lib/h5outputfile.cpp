#include "h5outputfile.h"

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

    ret = createExitPointsDatasets();
    if(!ret)
        return false;
    return true;
}

void H5OutputFile::appendTransmittedExitPoints(const MCfloat *buffer, const hsize_t size)
{
    if(!size)
        return;
    openDataSet("exit-points/transmitted");

    hsize_t dims[1];
    *dims = *extentDims();
    dims[0]--;

    writeHyperSlab(dims,&size,buffer);
}

bool H5OutputFile::createExitPointsDatasets()
{

    newGroup("exit-points");

    int ndims = 1;
    hsize_t dims[ndims];

    dims[0] = 1;
    bool ret = false;
    ret = newDataset("exit-points/reflected",ndims,dims,dims);
    if(!ret)
        return false;
    ret = newDataset("exit-points/transmitted",ndims,dims,dims);
    if(!ret)
        return false;
    return true;
}
