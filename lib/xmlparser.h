#ifndef XMLPARSER_H
#define XMLPARSER_H

#include <boost/property_tree/ptree.hpp>
#include "simulation.h"
#include <set>
#include <map>

using namespace boost::property_tree;
using namespace std;

class XMLParser
{
public:

    struct InvalidBool {string str;};

    XMLParser();

    void parseFile(const string &filename);
    void parseString(const string &xmlContent);
    bool showTrajectoryEnabled() const;
    Simulation *simulation() const;
    Sample *sample() const;

private:
    ptree pt;

    set<string> materialStrings;
    map<string, Material> materialMap;
    map<string, Source *> sourceMap;
    bool showTrajectory;
    Simulation *sim;
    Sample *_sample;
    Source *src;

    Material material(const string name) const;
    AbstractDistribution *distribution(const string name) const;
    bool boolean(const string str) const;
    void parse();
};

#endif // XMLPARSER_H
