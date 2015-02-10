#include "h5filehelper.h"
#include <stdio.h>

#include <string.h>

using namespace MCPP;

H5FileHelper::H5FileHelper(BaseObject *parent) :
    BaseObject(parent)
{
    Exception::getAutoPrint(Efunc,&EclientData);
    fName = NULL;
    dName = NULL;
    opened = false;
    file = NULL;
    dataSpace = NULL;
    dataSet = NULL;
    dims = NULL;
}

H5FileHelper::~H5FileHelper() {
    close();

    if(fName!=NULL) {
        free(fName);
        fName = NULL;
    }

    if(dName!=NULL) {
        free(dName);
        dName = NULL;
    }
}

/**
 * @brief Closes the HDF5 file
 */

void H5FileHelper::close() {
    if(!opened)
        return;

    closeDataSet();
    file->close();

    if(file!=NULL) {
        delete file;
        file = NULL;
    }

    opened = false;
}

const hsize_t *H5FileHelper::extentDims() const
{
    return dims;
}

/**
 * @brief Closes the current open dataset
 */

void H5FileHelper::closeDataSet() {
    if(dataSet!=NULL) {
        dataSpace->close();
        delete dataSpace;
        dataSpace = NULL;

        dataSet->close();
        delete dataSet;
        dataSet = NULL;

        if(dims!=NULL)
            free(dims);
        dims = NULL;
    }

    closeDataSet_impl();
}

/**
 * @brief Opens an HDF5 file
 * @param fileName Full path to the file to be opened
 * @param dataSetName The name of the dataset to be opened
 * @return true on success, false on error
 */

bool H5FileHelper::openFile(const char *fileName, const char* dataSetName)
{
    bool success = _openFile(fileName);
    copyToInternalVariable(&fName,fileName);
    if(!success)
        return false;
    success = openFile_impl();
    if(!success)
        return false;
    if(dataSetName != NULL)
        success = openDataSet(dataSetName);
    return success;
}

/**
 * @brief Opens an HDF5 file
 * @param fileName Full path to the file to be opened
 * @return true on success, false on error
 */

bool H5FileHelper::_openFile(const char *fileName) {
    close();
#ifdef PRINT_DEBUG_MSG
    logMessage("Opening %s... ",fileName);
#endif
    file = new H5File();
    try {
        if(!file->isHdf5(fileName)) {
            logMessage("is not a valid h5 file.\n");
            return false;
        }

        file->openFile(fileName,H5F_ACC_RDWR);
        opened = true;
    }
    catch ( Exception error )
    {
        logMessage("cannot open file.\n");
        return false;
    }
    return true;
}

/**
 * @brief Reopens the currently open HDF5 file
 */

void H5FileHelper::reopen() {
    _openFile(fName);
    _openDataSet(dName);
}

/**
 * @brief Creates a new HDF5 file
 * @param fileName Path to the new file to be created
 * @return
 */

bool H5FileHelper::newFile(const char *fileName) {
    close();
#ifdef PRINT_DEBUG_MSG
    logMessage("Creating new file %s... ",fileName);
#endif
    try{
        file = new H5File(fileName,H5F_ACC_TRUNC);
        opened = true;
    }
    catch (Exception error) {
        logMessage("Cannot create file\n");
        return false;
    }
#ifdef PRINT_DEBUG_MSG
    logMessage("done\n");
#endif
    return true;
}

#ifndef SWIG  //this is to work around a link error
bool H5FileHelper::newDataset(const char *datasetName, int ndims, const hsize_t *dims, const hsize_t *chunk_dim, PredType type)
{
#ifdef PRINT_DEBUG_MSG
    logMessage("Creating new dataset %s... ",datasetName);
#endif
    closeDataSet();

    double fillvalue = 0.;
    DSetCreatPropList plist;
    plist.setFillValue(type, &fillvalue);

    plist.setChunk(ndims,chunk_dim); //ndims is the rank

    hsize_t maxdims[ndims];
    memcpy(maxdims,dims,ndims*sizeof(hsize_t));
    maxdims[ndims-1] = H5S_UNLIMITED;

    try {
        dataSpace = new DataSpace (ndims, dims, maxdims);
        dataSet  = new DataSet(file->createDataSet(datasetName, type, *dataSpace, plist));
        openDataSet(datasetName);
    }
    catch (Exception error) {
        logMessage("Cannot create dataset %s.\n", datasetName);
        return false;
    }
#ifdef PRINT_DEBUG_MSG
    logMessage("done.\n");
#endif
    return true;
}
#endif

#ifndef SWIG  //this is to work around a link error
bool H5FileHelper::newDataset(const char *datasetName, int ndims, const hsize_t *dims, PredType type)
{
#ifdef PRINT_DEBUG_MSG
    logMessage("Creating new dataset %s... ",datasetName);
#endif
    closeDataSet();

    double fillvalue = 0.;
    DSetCreatPropList plist;
    plist.setFillValue(type, &fillvalue);

    hsize_t maxdims[ndims];
    memcpy(maxdims,dims,ndims*sizeof(hsize_t));

    try {
        dataSpace = new DataSpace (ndims, dims, maxdims);
        dataSet  = new DataSet(file->createDataSet(datasetName, type, *dataSpace, plist));
        openDataSet(datasetName);
    }
    catch (Exception error) {
        logMessage("Cannot create dataset %s.\n", datasetName);
        return false;
    }
#ifdef PRINT_DEBUG_MSG
    logMessage("done.\n");
#endif
    return true;
}
#endif

/**
 * @brief Checks for the existence of a particular dataset
 * @param dataSetName Name of the dataset
 * @return true if the dataset exists, false otherwise
 */

bool H5FileHelper::dataSetExists(const char *dataSetName) const
{
    return H5Lexists(file->getId(),dataSetName,H5P_DEFAULT);
}

/**
 * @brief Opens a dataset in the current file
 * @param dataSetName
 * @return
 */

bool H5FileHelper::openDataSet(const char *dataSetName) {
    closeDataSet();
#ifdef PRINT_DEBUG_MSG
    logMessage("Opening dataset %s... ",dataSetName);
#endif
    if(!dataSetExists(dataSetName)) {
        logMessage("Dataset %s does not exist.\n",dataSetName);
        return false;
    }

    copyToInternalVariable(&dName,dataSetName);

    _openDataSet(dName);

    ndims = dataSpace->getSimpleExtentNdims();
    dims = (hsize_t *)malloc(ndims*sizeof(hsize_t));
    dataSpace->getSimpleExtentDims(dims);

    return true;
}

void H5FileHelper::newGroup(const char *name)
{
    file->createGroup(name);
}

/**
 * @brief Opens a dataset
 * @param dataSetName Name of the dataset
 */

void H5FileHelper::_openDataSet(const char *dataSetName) {
    dataSet = new DataSet (file->openDataSet(dataSetName));
    dataSpace = new DataSpace (dataSet->getSpace());
}

bool H5FileHelper::openFile_impl()
{
    return true;
}

void H5FileHelper::closeDataSet_impl()
{

}

/**
 * @brief Loads a hyperslab from the current dataset
 * @param start Offset of the start of hyperslab
 * @param count Number of blocks included in the hyperslab
 * @param destBuffer the destination buffer for the hyperslab
 *
 * \pre destBuffer must be valid and big enough to hold all the hyperslab data.
 */

void H5FileHelper::loadHyperSlab(const hsize_t *start, const hsize_t *count, MCfloat *destBuffer) {
    hsize_t _start[ndims];
    for (int i = 0; i < ndims; ++i) {
        _start[i] = 0;
    }
    DataSpace memspace( ndims, count );
    memspace.selectHyperslab(H5S_SELECT_SET, count, _start);
#ifdef PRINT_DEBUG_MSG
    for (int i = 0; i < ndims; ++i) {
        logMessage("loadHyperSlab dim%d: [%llu %llu]",i,start[i],start[i]+count[i]-1);
    }
#endif
    dataSpace->selectHyperslab(H5S_SELECT_SET, count, start);
    dataSet->read(destBuffer,MCH5FLOAT,memspace,*dataSpace);
#ifdef PRINT_DEBUG_MSG
    logMessage("done.");
#endif
}

/**
 * @brief Loads the whole data in the current dataset into memory
 * @param destBuffer the destination buffer
 *
 * \pre destBuffer must be valid and big enough to hold all the data.
 */

void H5FileHelper::loadAll(MCfloat *destBuffer) {
#ifdef PRINT_DEBUG_MSG
    logMessage("Loading whole dataset...");
#endif
    dataSet->read(destBuffer,MCH5FLOAT,H5S_ALL,H5S_ALL);
#ifdef PRINT_DEBUG_MSG
    logMessage("done.");
#endif
}

/**
 * @brief Writes a hyperslab in the current dataset
 * @param start Offset of the start of hyperslab
 * @param count Number of blocks to be written
 * @param srcBuffer The buffer where the data to be written is stored
 */

void H5FileHelper::writeHyperSlab(const hsize_t *start, const hsize_t *count, const MCfloat *srcBuffer) {
    hsize_t extDims[ndims];
    for (int i = 0; i < ndims; ++i) {
        extDims[i] = start[i] + count[i];
    }

    for (int i = 0; i < ndims; ++i) {
        if(extDims[i] > dims[i]) {
            dataSet->extend(extDims);
            break;
        }
    }

    *dataSpace = dataSet->getSpace();
    dataSpace->getSimpleExtentDims(dims);

#ifdef PRINT_DEBUG_MSG
    for (int i = 0; i < ndims; ++i) {
        logMessage("writeHyperSlab dim%d: [%llu %llu]",i,start[i],start[i]+count[i]-1);
    }
#endif

    hsize_t _start[ndims];
    for (int i = 0; i < ndims; ++i)
        _start[i] = 0;

    DataSpace memspace( ndims, count );
    memspace.selectHyperslab(H5S_SELECT_SET, count, _start);
    dataSpace->selectHyperslab(H5S_SELECT_SET, count, start);
    dataSet->write(srcBuffer,MCH5FLOAT,memspace,*dataSpace);
}

/**
 * @brief Writes a hyperslab in the current dataset
 * @param start Offset of the start of hyperslab
 * @param count Number of blocks to be written
 * @param srcBuffer The buffer where the data to be written is stored
 */

void H5FileHelper::writeHyperSlab(const hsize_t *start, const hsize_t *count, const u_int64_t *srcBuffer) {
    hsize_t extDims[ndims];
    for (int i = 0; i < ndims; ++i) {
        extDims[i] = start[i] + count[i];
    }

    dataSet->extend(extDims);

    *dataSpace = dataSet->getSpace();
    dataSpace->getSimpleExtentDims(dims);

#ifdef PRINT_DEBUG_MSG
    for (int i = 0; i < ndims; ++i) {
        logMessage("writeHyperSlab dim%d: [%llu %llu]",i,start[i],start[i]+count[i]-1);
    }
#endif

    hsize_t _start[ndims];
    for (int i = 0; i < ndims; ++i)
        _start[i] = 0;

    DataSpace memspace( ndims, count );
    memspace.selectHyperslab(H5S_SELECT_SET, count, _start);
    dataSpace->selectHyperslab(H5S_SELECT_SET, count, start);
    dataSet->write(srcBuffer,PredType::NATIVE_UINT64,memspace,*dataSpace);
}

void H5FileHelper::copyToInternalVariable(char **dest, const char *src) {
    if(*dest!=NULL)
        free(*dest);
    *dest = (char*)malloc(sizeof(char)*strlen(src)+1);
    memcpy(*dest,src,strlen(src)+1);
}

void H5FileHelper::resetErrorAutoPrint() {
    Exception::setAutoPrint(Efunc,EclientData);
}

/**
 * @brief Returns the rank of the current dataset
 * @return
 */

int H5FileHelper::getRank() const {
    return ndims;
}

/**
 * @brief Returns a pointer to a string containing the current dataset name
 * @return
 */

const char* H5FileHelper::getDataSetName() const {
    return dName;
}

const char *H5FileHelper::currentDataset() const {
    return dName;
}
