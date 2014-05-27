#ifndef H5FILEHELPER_H
#define H5FILEHELPER_H

#include "baseobject.h"
#include <H5Cpp.h>

#ifdef DOUBLEPRECISION
#define MCH5FLOAT PredType::NATIVE_DOUBLE
#else
#define MCH5FLOAT PredType::NATIVE_FLOAT
#endif

#define PRINT_DEBUG_MSG

using namespace H5;

/**
 * @brief The H5FileHelper class is a convenience wrapper around H5::H5File.
 */

class H5FileHelper : public BaseObject
{
public:
    H5FileHelper(BaseObject *parent=NULL);
    virtual ~H5FileHelper();
    bool openFile(const char *fileName, const char *dataSetName=NULL);
    void reopen();
    bool openDataSet(const char *dataSetName);
    void newGroup(const char *name);
    virtual bool newFile(const char *fileName);
    bool newDataset(const char *datasetName, int ndims, const hsize_t *dims, const hsize_t *chunk_dim, PredType type=MCH5FLOAT);
    bool dataSetExists(const char *dataSetName) const;
    void loadHyperSlab(const hsize_t *start, const hsize_t *count, MCfloat *destBuffer);
    void writeHyperSlab(const hsize_t *start, const hsize_t *count, const MCfloat *srcBuffer);
    void writeHyperSlab(const hsize_t *start, const hsize_t *count, const u_int64_t *srcBuffer);
    void loadAll(MCfloat *destBuffer);
    void close();
    void closeDataSet();
    const hsize_t *extentDims() const;

    int getRank() const;
    const char *getDataSetName() const;
    const char *currentDataset() const;

protected:
    H5File *file;
    DataSet *dataSet;
    DataSpace *dataSpace;
    int ndims;
    hsize_t *dims;

private:
    char *fName, *dName;
    bool opened;

    H5E_auto2_t Efunc;
    void *EclientData;

    void copyToInternalVariable(char **dest, const char *src);
    void resetErrorAutoPrint();
    bool _openFile(const char *fileName);
    void _openDataSet(const char *dataSetName);
    virtual bool openFile_impl();
    virtual void closeDataSet_impl();
};

#endif // H5FILEHELPER_H
