#ifndef H5OUTPUTFILE_H
#define H5OUTPUTFILE_H

#include "xmlparser.h"
#include "h5filehelper.h"

/**
 * @brief The H5OutputFile class allows to manipulate the HDF5 files generated by
 *        MCPlusPlus
 */

class H5OutputFile : public H5FileHelper
{
public:
    H5OutputFile();
    virtual ~H5OutputFile();

    virtual bool newFile(const char *fileName, bool create_datasets=true);
    bool newFromXML(const char *xmlFile, const char *fileName);
    void appendExitKVectors(walkerType type, const MCfloat *buffer, const hsize_t size);
    void appendExitPoints(walkerType type, const MCfloat *buffer, const hsize_t size);
    void appendWalkTimes(walkerType type, const MCfloat *buffer, const hsize_t size);
    void loadExitPoints(walkerType type, MCfloat *destBuffer, const hsize_t *start=NULL, const hsize_t *count=NULL);
    void loadWalkTimes(walkerType type, MCfloat *destBuffer, const hsize_t *start=NULL, const hsize_t *count=NULL);
    void loadExitKVectors(walkerType type, MCfloat *destBuffer, const hsize_t *start=NULL, const hsize_t *count=NULL);

    void saveRNGState(const uint seed, const string s);
    string readRNGState(const uint seed) const;
    void appendPhotonCounts(const u_int64_t transmitted, const u_int64_t ballistic, const u_int64_t reflected, const u_int64_t backReflected);
    u_int64_t transmitted() const;
    u_int64_t ballistic() const;
    u_int64_t reflected() const;
    u_int64_t backReflected() const;
    const u_int64_t *photonCounters() const;
    void writeXMLDescription(const char *inputFile);
    string readXMLDescription();
    Simulation *simulation() const;
    XMLParser *xmlParser() const;
    void setXMLParserEnabled(bool enable);

private:
    bool createDatasets(uint walkTimesSaveFlags, uint exitPointsSaveFlags, uint exitKVectorsSaveFlags);
    bool createRNGDataset(uint seed);
    void appendTo1Ddataset(const char *datasetName, const MCfloat *buffer, const hsize_t size);
    void loadFrom1Ddataset(const char *datasetName, MCfloat *destBuffer, const hsize_t *start, const hsize_t *count);
    void writeVLenString(const char *datasetName, const string str);
    string readVLenString(const char *datasetName) const;
    bool openFile_impl();

    u_int64_t _photonCounters[4];

    XMLParser *_parser;
    bool XMLParserEnabled;
};

#endif // H5OUTPUTFILE_H
