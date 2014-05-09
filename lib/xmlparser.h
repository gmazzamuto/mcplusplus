#ifndef XMLPARSER_H
#define XMLPARSER_H

#include <boost/property_tree/ptree.hpp>
#include "simulation.h"
#include <set>
#include <map>

using namespace boost::property_tree;
using namespace std;

/**
 * @brief The XMLParser class parses the XML file describing a simulation.
 */

class XMLParser
{
public:

    struct InvalidBool {string str;};

    XMLParser();

    void parseFile(const string &filename);
    void parseString(const string &xmlContent);
    bool showTrajectoryEnabled() const;
    void setXMLFile(const string &filename);
    void setXMLContent(const string &xmlContent);
    Simulation *simulation() const;
    Sample *sample() const;
    void parseAll();
    void parseOutput();
    uint walkTimesSaveFlags() const;
    uint exitPointsSaveFlags() const;
    uint exitKVectorsSaveFlags() const;
    uint exitKVectorsDirsSaveFlags() const;
    const string outputFileName() const;
    unsigned int walkerSaveFlags(const string flags) const;
    unsigned int dirSaveFlags(const string flags) const;

private:
    ptree pt;
    ptree::const_assoc_iterator it;

    set<string> materialStrings;
    map<string, Material*> materialMap;
    map<string, Source *> sourceMap;
    bool showTrajectory;
    Simulation *sim;
    Sample *_sample;
    Source *src;
    string _outputFileName;

    Material *material(const string name) const;
    AbstractDistribution *distribution(const string name) const;
    bool boolean(const string str) const;

    unsigned int _walkTimesSaveFlags, _exitPointsSaveFlags, _exitKVectorsDirsSaveFlags, _exitKVectorsSaveFlags;
    void parseMaterials();
};

#endif // XMLPARSER_H
