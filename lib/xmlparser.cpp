#include "xmlparser.h"

#include <boost/property_tree/exceptions.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/optional.hpp>

using namespace boost::math::constants;
using namespace boost;

XMLParser::XMLParser()
{
    showTrajectory = false;
    sim = NULL;
    src = NULL;
    _sample = NULL;
}

void XMLParser::parseFile(const string &filename)
{
    setXMLFile(filename);
    parseAll();
}

void XMLParser::parseString(const string &xmlContent)
{
    setXMLContent(xmlContent);
    parseAll();
}

void XMLParser::parseAll() {

    //materials
    parseMaterials();

    //sample
    if(_sample != NULL)
        delete _sample;
    _sample = new Sample();
    string left = pt.get<string>("MCPlusPlus.MLSample.<xmlattr>.left");
    string right = pt.get<string>("MCPlusPlus.MLSample.<xmlattr>.right");
    _sample->setSurroundingEnvironment(material(left),material(right));

    vector<Material*> preLayerMaterial;
    vector<MCfloat> preLayerThickness;
    BOOST_FOREACH(ptree::value_type &v, pt.get_child("MCPlusPlus.MLSample")) {
        if(v.first == "layer" || v.first == "prelayer") {
            Material *mat = material(v.second.get_child("<xmlattr>.material").data());
            if(v.first == "layer")
                _sample->addLayer(mat,v.second.get<MCfloat>("<xmlattr>.thickness"));
            else if(v.first == "prelayer") {
                preLayerMaterial.push_back(mat);
                preLayerThickness.push_back(v.second.get<MCfloat>("<xmlattr>.thickness"));
            }
        }
    }

    for (int i = preLayerMaterial.size() - 1; i >= 0; --i) {
        _sample->addPreLayer(preLayerMaterial[i],preLayerThickness[i]);
    }


    //source
    string srcType = pt.get<string>("MCPlusPlus.source.<xmlattr>.type", "__default__");
    if(src != NULL)
        delete src;
    if(srcType == "__default__") {
        src = new Source();
        AbstractDistribution *rDistr[3];
        rDistr[0] = distribution(pt.get<string>("MCPlusPlus.source.<xmlattr>.rx"));
        rDistr[1] = distribution(pt.get<string>("MCPlusPlus.source.<xmlattr>.ry"));

        src->setr0Distribution(rDistr[0], rDistr[1], pt.get<MCfloat>("MCPlusPlus.source.<xmlattr>.rz"));

        AbstractDistribution *cosTheta = distribution(pt.get<string>("MCPlusPlus.source.<xmlattr>.cosTheta"));
        AbstractDistribution *psi = distribution(pt.get<string>("MCPlusPlus.source.<xmlattr>.psi"));

        src->setk0Distribution(cosTheta,psi);
    }
    else if(srcType == "IsotropicPointSource") {
        src = new IsotropicPointSource(pt.get<MCfloat>("MCPlusPlus.source.<xmlattr>.rz"));
    }
    else if(srcType == "PencilBeamSource") {
        src = new PencilBeamSource();
    }

    it = pt.find("MCPlusPlus.source.<xmlattr>.wavelength");
    if(it != pt.not_found())
        src->setWavelength(boost::lexical_cast<MCfloat>(it->second.data()));

    AbstractDistribution *walkTime = distribution(pt.get<string>("MCPlusPlus.source.<xmlattr>.walkTime"));
    src->setWalkTimeDistribution(walkTime);

    //simulation
    if(sim!=NULL)
        delete sim;
    sim = new Simulation();
    sim->setSample(_sample);
    sim->setSource(src);

    try {
        ptree v = pt.get_child("MCPlusPlus.simulation");
        string str = v.get<string>("<xmlattr>.show-trajectory", "__default__");
        if(str != "__default__") {
            showTrajectory = boolean(str);
            sim->setSaveTrajectoryEnabled(showTrajectory);
        }
        sim->setTimeOriginZ(v.get<MCfloat>("<xmlattr>.time-origin-z", 0));
    } catch (ptree_bad_path) {
    }

    //output
    parseOutput();
    sim->setExitPointsSaveFlags(_exitPointsSaveFlags);
    sim->setWalkTimesSaveFlags(_walkTimesSaveFlags);
    sim->setExitKVectorsSaveFlags(_exitKVectorsSaveFlags);
    sim->setExitKVectorsDirsSaveFlags(_exitKVectorsDirsSaveFlags);
}

void XMLParser::parseOutput()
{
    try {
        ptree v = pt.get_child("MCPlusPlus.output");
        string str = v.get<string>("<xmlattr>.exit-points", "__default__");
        if(str=="__default__")
            _exitPointsSaveFlags = 0;
        else
            _exitPointsSaveFlags = walkerSaveFlags(str);
        str = v.get<string>("<xmlattr>.walk-times", "__default__");
        if(str=="__default__")
            _walkTimesSaveFlags = 0;
        else
            _walkTimesSaveFlags = walkerSaveFlags(str);
        str = v.get<string>("<xmlattr>.exit-k-dirs", "__default__");
        if(str=="__default__")
            _exitKVectorsDirsSaveFlags = 0;
        else
            _exitKVectorsDirsSaveFlags = dirSaveFlags(str);
        str = v.get<string>("<xmlattr>.exit-k", "__default__");
        if(str=="__default__")
            _exitKVectorsSaveFlags = 0;
        else
            _exitKVectorsSaveFlags = walkerSaveFlags(str);
    } catch (ptree_bad_path) {
    }
}

uint XMLParser::walkTimesSaveFlags() const
{
    return _walkTimesSaveFlags;
}

uint XMLParser::exitPointsSaveFlags() const
{
    return _exitPointsSaveFlags;
}

uint XMLParser::exitKVectorsSaveFlags() const
{
    return _exitKVectorsSaveFlags;
}

uint XMLParser::exitKVectorsDirsSaveFlags() const
{
    return _exitKVectorsDirsSaveFlags;
}

bool XMLParser::showTrajectoryEnabled() const
{
    return showTrajectory;
}

void XMLParser::setXMLFile(const string &filename)
{
    read_xml(filename, pt, xml_parser::no_comments);
}

void XMLParser::setXMLContent(const string &xmlContent)
{
    if(xmlContent.empty())
        return;
    stringstream ss;
    ss << xmlContent;
    read_xml(ss, pt, xml_parser::no_comments);
}

Simulation *XMLParser::simulation() const
{
    return sim;
}

Sample *XMLParser::sample() const
{
    return _sample;
}

Material* XMLParser::material(const string name) const
{
    if(name == "Air")
        return new Air();
    else if(name == "Vacuum")
        return new Vacuum();
    else if(name == "GlassSlide")
        return new GlassSlide();
    else if(name == "NorlandOpticalAdhesive65")
        return new NorlandOpticalAdhesive65();
    else
        return materialMap.at(name);
}

AbstractDistribution *XMLParser::distribution(const string name) const
{
    if(name == "uniform_0_2pi") {
        return new UniformDistribution(0,two_pi<MCfloat>());
    }
    else if (name == "uniform_0_pi") {
        return new UniformDistribution(0,pi<MCfloat>());
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

unsigned int XMLParser::walkerSaveFlags(const string flags) const {
    unsigned int ret = 0;
    for (unsigned int i = 0; i < flags.size(); ++i) {
        switch (flags.at(i)) {
        case 't':
            ret |= FLAG_TRANSMITTED;
            break;
        case 'b':
            ret |= FLAG_BALLISTIC;
            break;
        case 'r':
            ret |= FLAG_REFLECTED;
            break;
        case 'k':
            ret |= FLAG_BACKREFLECTED;
            break;
        default:
            break;
        }
    }

    return ret;
}

unsigned int XMLParser::dirSaveFlags(const string flags) const
{
    unsigned int ret = 0;
    for (unsigned int i = 0; i < flags.size(); ++i) {
        switch (flags.at(i)) {
        case 'x':
            ret |= DIR_X;
            break;
        case 'y':
            ret |= DIR_Y;
            break;
        case 'z':
            ret |= DIR_Z;
            break;
        default:
            break;
        }
    }

    return ret;
}

void XMLParser::parseMaterials()
{
    optional<MCfloat> element;
    map<string,Material*>::iterator map_it = materialMap.begin();
    BOOST_FOREACH(ptree::value_type &v, pt.get_child("MCPlusPlus.materials")) {
        if(v.first != "material")
            continue;
        string name = v.second.get_child("<xmlattr>.name").data();
        Material *mat = new Material();
        it = v.second.find("ls");
        element = v.second.get_optional<MCfloat>("<xmlattr>.ls");
        if(element.is_initialized())
            mat->ls = element.get();
        element = v.second.get_optional<MCfloat>("<xmlattr>.g");
        if(element.is_initialized())
            mat->g = element.get();
        element = v.second.get_optional<MCfloat>("<xmlattr>.n");
        if(element.is_initialized())
            mat->n = element.get();
        materialMap.insert(map_it, pair<string,Material*>(name,mat));
    }
}
