#ifndef H5FILEHELPER_H
#define H5FILEHELPER_H

#include "baseobject.h"
#include <H5Cpp.h>

using namespace H5;

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
    bool dataSetExists(const char *dataSetName);
    void loadHyperSlab(const hsize_t *start, const hsize_t *count, double *destBuffer);
    void writeHyperSlab(const hsize_t *start, const hsize_t *count, const double *srcBuffer);
    void loadAll(double *destBuffer);
    void close();
    const hsize_t *extentDims() const;

    int getRank() const;
    const char *getDataSetName() const;
    const char *currentDataset() const;

private:
    H5File *file;
    DataSet *dataSet;
    DataSpace *dataSpace;
    char *fName, *dName;
    bool opened;
    int ndims;
    hsize_t *dims;

    H5E_auto2_t Efunc;
    void *EclientData;

    void copyToInternalVariable(char **dest, const char *src);
    void closeDataSet();
    void resetErrorAutoPrint();
    bool _openFile(const char *fileName);
    void _openDataSet(const char *dataSetName);
};

#endif // H5FILEHELPER_H
