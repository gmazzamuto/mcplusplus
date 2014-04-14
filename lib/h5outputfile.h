#ifndef H5OUTPUTFILE_H
#define H5OUTPUTFILE_H

#include "simulation.h"
#include "h5filehelper.h"

/**
 * @brief The H5OutputFile class allows to manipulate the HDF5 files generated by
 *        MCPlusPlus
 */

class H5OutputFile : public H5FileHelper
{
public:
    H5OutputFile();

    virtual bool newFile(const char *fileName);
    void appendTransmittedExitPoints(const MCfloat *buffer, const hsize_t size);
    void appendReflectedExitPoints(const MCfloat *buffer, const hsize_t size);
    void appendTransmittedWalkTimes(const MCfloat *buffer, const hsize_t size);
    void appendReflectedWalkTimes(const MCfloat *buffer, const hsize_t size);
    void loadTransmittedExitPoints(MCfloat *destBuffer, const hsize_t *start=NULL, const hsize_t *count=NULL);
    void loadReflectedExitPoints(MCfloat *destBuffer, const hsize_t *start=NULL, const hsize_t *count=NULL);
    void loadTransmittedWalkTimes(MCfloat *destBuffer, const hsize_t *start=NULL, const hsize_t *count=NULL);
    void loadReflectedWalkTimes(MCfloat *destBuffer, const hsize_t *start=NULL, const hsize_t *count=NULL);
    void saveRNGState(const string s);
    string readRNGState();
    unsigned long int transmitted();
    unsigned long int reflected();
    void writeXMLDescription(const char *inputFile);
    string readXMLDescription();
    Simulation *simulation();

private:
    bool createDatasets();
    void appendTo1Ddataset(const char *datasetName, const MCfloat *buffer, const hsize_t size);
    void loadFrom1Ddataset(const char *datasetName, MCfloat *destBuffer, const hsize_t *start, const hsize_t *count);
    void writeVLenString(const char *datasetName, const string str);
    string readVLenString(const char *datasetName);

    Simulation *sim;
};

#endif // H5OUTPUTFILE_H