/* File : pymcplusplus.i */
%module pymcplusplus

namespace MCPP {}

%{
#include <MCPlusPlus/baseobject.h>
#include <MCPlusPlus/baserandom.h>
#include <MCPlusPlus/costhetagenerator.h>
#include <MCPlusPlus/distributions.h>
#include <MCPlusPlus/material.h>
#include <MCPlusPlus/sample.h>
#include <MCPlusPlus/gaussianraybundlesource.h>
#include <MCPlusPlus/MCglobal.h>
#include <MCPlusPlus/psigenerator.h>
#include <MCPlusPlus/histogram.h>
#include <MCPlusPlus/simulation.h>
#include <MCPlusPlus/source.h>
#include <MCPlusPlus/walker.h>

#include <MCPlusPlus/h5filehelper.h>
#include <MCPlusPlus/h5outputfile.h>

#include <H5Cpp.h>
#include <boost/random.hpp>
#include <boost/property_tree/ptree.hpp>
#include <MCPlusPlus/h5filehelper.h>
#include <MCPlusPlus/h5outputfile.h>

using namespace MCPP;
%}


namespace boost {
  namespace random {}
  namespace property_tree {}
}

namespace H5 {}

typedef long long int u_int64_t;

%include "include/MCPlusPlus/walker.h"
%include "include/MCPlusPlus/baseobject.h"
%include "include/MCPlusPlus/baserandom.h"
%include "include/MCPlusPlus/distributions.h"
%include "include/MCPlusPlus/costhetagenerator.h"
%include "include/MCPlusPlus/psigenerator.h"
%include "include/MCPlusPlus/source.h"
%include "include/MCPlusPlus/material.h"
%include "include/MCPlusPlus/sample.h"
%include "include/MCPlusPlus/gaussianraybundlesource.h"
%include "include/MCPlusPlus/MCglobal.h"
%include "include/MCPlusPlus/histogram.h"
%include "include/MCPlusPlus/simulation.h"
%include <boost/random.hpp>
%include <boost/property_tree/ptree.hpp>
%include "include/MCPlusPlus/h5filehelper.h"
%include "include/MCPlusPlus/h5outputfile.h"
