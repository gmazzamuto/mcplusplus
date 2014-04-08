#include "xmlparser.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/exceptions.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include <stdlib.h>

using namespace boost::property_tree;
using namespace boost::math::constants;

XMLParser::XMLParser()
{
    showTrajectory = false;
}

Simulation* XMLParser::load(const std::string &filename)
{
    using boost::property_tree::ptree;

    ptree pt;

    read_xml(filename, pt, xml_parser::no_comments);


    //materials
    map<string,Material>::iterator it = materialMap.begin();
    BOOST_FOREACH(ptree::value_type &v, pt.get_child("MCPlusPlus.materials")) {
        string name = v.second.get_child("<xmlattr>.name").data();
        Material mat;
        mat.ls = v.second.get<MCfloat>("<xmlattr>.ls");
        mat.g = v.second.get<MCfloat>("<xmlattr>.g");
        mat.n = v.second.get<MCfloat>("<xmlattr>.n");
        materialMap.insert(it, pair<string,Material>(name,mat));
    }


    //sample
    Sample *sample = new Sample();
    string left = pt.get<string>("MCPlusPlus.MLSample.<xmlattr>.left");
    string right = pt.get<string>("MCPlusPlus.MLSample.<xmlattr>.right");
    sample->setSurroundingEnvironment(material(left),material(right));

    BOOST_FOREACH(ptree::value_type &v, pt.get_child("MCPlusPlus.MLSample")) {
        if(v.first != "layer")
            continue;
        string str = v.second.get_child("<xmlattr>.material").data();
        sample->addLayer(material(str),v.second.get<MCfloat>("<xmlattr>.thickness"));
    }

    BOOST_FOREACH(ptree::value_type &v, pt.get_child("MCPlusPlus")) {
        if(v.first != "layer")
            continue;
        string str = v.second.get_child("<xmlattr>.material").data();
        sample->addLayer(material(str),v.second.get<MCfloat>("<xmlattr>.thickness"));
    }


    //source
    string srcType = pt.get<string>("MCPlusPlus.MLSample.source.type", "__default__");
    Source *src = NULL;
    if(srcType == "__default__") {
        src = new Source();
        AbstractDistribution *rDistr[3];
        rDistr[0] = distribution(pt.get<string>("MCPlusPlus.source.<xmlattr>.rx"));
        rDistr[1] = distribution(pt.get<string>("MCPlusPlus.source.<xmlattr>.ry"));
        rDistr[2] = distribution(pt.get<string>("MCPlusPlus.source.<xmlattr>.rz"));

        src->setr0Distribution(rDistr);

        AbstractDistribution *cosTheta = distribution(pt.get<string>("MCPlusPlus.source.<xmlattr>.cosTheta"));
        AbstractDistribution *psi = distribution(pt.get<string>("MCPlusPlus.source.<xmlattr>.psi"));

        src->setk0Distribution(cosTheta,psi);

        AbstractDistribution *walkTime = distribution(pt.get<string>("MCPlusPlus.source.<xmlattr>.walkTime"));
        src->setWalkTimeDistribution(walkTime);
    }


    //simulation
    Simulation *sim = new Simulation();
    sim->setSample(sample);
    sim->setSource(src);

    try {
        ptree v = pt.get_child("MCPlusPlus.simulation");
        string str = v.get<string>("<xmlattr>.showTrajectory", "__default__");
        showTrajectory = boolean(str);
        sim->setSaveTrajectoryEnabled(boolean(str));
    } catch (ptree_bad_path) {
    }

    return sim;
}

bool XMLParser::showTrajectoryEnabled() const
{
    return showTrajectory;
}

Material XMLParser::material(const string name) const
{
    if(name == "Air")
       return Air();
    else if(name == "Vacuum")
        return Vacuum();
    else if(name == "GlassSlide")
        return GlassSlide();
    else if(name == "NorlandOpticalAdhesive65")
        return NorlandOpticalAdhesive65();
    else
        return materialMap.at(name);
}

AbstractDistribution *XMLParser::distribution(const string name) const
{
    if(name == "uniform_0_2pi") {
        return new UniformDistribution(0,two_pi<MCfloat>());
    }
    else
        return new DeltaDistribution(atof(name.c_str()));
}

bool XMLParser::boolean(const string str) const
{
    if(str == "true")
        return true;
    else if(str == "false")
        return false;
    else {
        struct InvalidBool e;
        e.str = str;
        throw e;
    };
}
