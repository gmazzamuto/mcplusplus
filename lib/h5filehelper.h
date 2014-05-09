#ifndef H5FILEHELPER_H
#define H5FILEHELPER_H

#include "baseobject.h"
#include <H5Cpp.h>

//#define PRINT_DEBUG_MSG

using namespace H5;

/**
 * @brief The H5FileHelper class is a convenience wrapper around H5::H5File.
 */

class H5FileHelper : public BaseObject
{
public:
    H5FileHelper();
    virtual ~H5FileHelper();
    bool openFile(const char *fileName, const char *dataSetName=NULL);
    void reopen();
    bool openDataSet(const char *dataSetName);
    void newGroup(const char *name);
    virtual bool newFile(const char *fileName);
    bool newDataset(const char *datasetName, int ndims, const hsize_t *dims, const hsize_t *chunk_dim);
    bool dataSetExists(const char *dataSetName) const;
    void loadHyperSlab(const hsize_t *start, const hsize_t *count, MCfloat *destBuffer);
    void writeHyperSlab(const hsize_t *start, const hsize_t *count, const MCfloat *srcBuffer);
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

private:
    char *fName, *dName;
    bool opened;
    int ndims;
    hsize_t *dims;

    H5E_auto2_t Efunc;
    void *EclientData;

    void copyToInternalVariable(char **dest, const char *src);
    void resetErrorAutoPrint();
    bool _openFile(const char *fileName);
    void _openDataSet(const char *dataSetName);
    virtual bool openFile_impl();
};

#endif // H5FILEHELPER_H
