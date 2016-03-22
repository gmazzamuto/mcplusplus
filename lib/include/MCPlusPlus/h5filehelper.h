/*
  This file is part of MCPlusPlus.

  MCPlusPlus is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Mcplusplus is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with MCPlusPlus.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef H5FILEHELPER_H
#define H5FILEHELPER_H

#include "baseobject.h"
#include <H5Cpp.h>

#define MCH5FLOAT PredType::NATIVE_DOUBLE

//#define PRINT_DEBUG_MSG

using namespace H5;

namespace MCPP {

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
#ifndef SWIG //this is to work around a link error
    bool newDataset(const char *datasetName, int ndims, const hsize_t *dims, const hsize_t *chunk_dim, PredType type=MCH5FLOAT);
    bool newDataset(const char *datasetName, int ndims, const hsize_t *dims, PredType type=MCH5FLOAT);
#endif
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
    void writeColumnNames(const uint nCols, string *vect);

    void writeHyperSlabDouble(const hsize_t *start, const hsize_t *count, const double *srcBuffer);
protected:
    H5File *file;
    DataSet *dataSet;
    DataSpace *dataSpace;
    int ndims;
    hsize_t *dims;

private:
    void resetErrorAutoPrint();
    bool _openFile(const char *fileName);
    void _openDataSet(const char *dataSetName);
    virtual bool openFile_impl();
    virtual void closeDataSet_impl();

    char *fName, *dName;
    bool opened, dsOpened;

    H5E_auto2_t Efunc;
    void *EclientData;
};

}
#endif // H5FILEHELPER_H
