#ifndef XMLPARSER_H
#define XMLPARSER_H

#include "simulation.h"
#include <set>
#include <map>

using namespace std;

class XMLParser
{
public:
    XMLParser();

    Simulation *load(const std::string &filename);

private:
    set<string> materialStrings;

    map<string, Material> materialMap;
    map<string, Source *> sourceMap;

    Material material(const string name) const;
    AbstractDistribution *distribution(const string name) const;
};

#endif // XMLPARSER_H
