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

void H5OutputFile::loadTransmittedExitPoints(MCfloat *destBuffer)
{
    openDataSet("exit-points/transmitted");
    loadAll(destBuffer);
}

void H5OutputFile::loadTransmittedExitPoints(const hsize_t *start, const hsize_t *count, MCfloat *destBuffer)
{
    openDataSet("exit-points/transmitted");
    loadHyperSlab(start,count,destBuffer);
}

unsigned long H5OutputFile::transmitted()
{
    openDataSet("exit-points/transmitted");
    return extentDims()[0]/2;
}

void H5OutputFile::writeXMLDescription(const char *inputFile)
{
    //read whole file to string
    std::ifstream f(inputFile);
    std::string str((std::istreambuf_iterator<char>(f)),
                    std::istreambuf_iterator<char>());

    hid_t       filetype, memtype, space, dset;
    hsize_t     dims[1] = {1};

    filetype = H5Tcopy (H5T_C_S1);
    H5Tset_size (filetype, str.size());
    memtype = H5Tcopy (H5T_C_S1);
    H5Tset_size (memtype, str.size());

    space = H5Screate_simple (1, dims, NULL);

    dset = H5Dcreate (file->getId(), "XMLDescription", filetype, space, H5P_DEFAULT);
    H5Dwrite (dset, memtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, str.data());

    H5Dclose (dset);
    H5Sclose (space);
    H5Tclose (filetype);
    H5Tclose (memtype);
}

string H5OutputFile::loadXMLDescription()
{
    string str;
    if(!dataSetExists("XMLDescription"))
        return str;
    hid_t       filetype, memtype, space, dset;
    size_t      sdim;

    dset = H5Dopen (file->getId(), "XMLDescription");

    filetype = H5Dget_type (dset);
    sdim = H5Tget_size (filetype) + 1; //including '\0'

    space = H5Dget_space (dset);

    char buf[sdim];

    memtype = H5Tcopy (H5T_C_S1);
    H5Tset_size (memtype, sdim);

    H5Dread (dset, memtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, buf);

    H5Dclose (dset);
    H5Sclose (space);
    H5Tclose (filetype);
    H5Tclose (memtype);

    str = buf;
    return str;
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
