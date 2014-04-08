#ifndef XMLPARSER_H
#define XMLPARSER_H

#include "simulation.h"
#include <set>
#include <map>

using namespace std;

class XMLParser
{
public:

    struct InvalidBool {string str;};

    XMLParser();

    Simulation *load(const std::string &filename);
    bool showTrajectoryEnabled() const;

private:
    set<string> materialStrings;

    map<string, Material> materialMap;
    map<string, Source *> sourceMap;
    bool showTrajectory;

    Material material(const string name) const;
    AbstractDistribution *distribution(const string name) const;
    bool boolean(const string str) const;
};

#endif // XMLPARSER_H
