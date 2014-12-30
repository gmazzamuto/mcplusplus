/* File : pymcplusplus.i */
%module pymcplusplus

namespace MCPP {}

%{
#include "baseobject.h"
#include "baserandom.h"
#include "costhetagenerator.h"
#include "distributions.h"
#include "gaussianraybundlesource.h"
#include "material.h"
#include "MCglobal.h"
#include "psigenerator.h"
#include "sample.h"
#include "simulation.h"
#include "source.h"
#include "walker.h"

#include "h5filehelper.h"
#include "h5outputfile.h"

#include <H5Cpp.h>
#include <boost/random.hpp>
#include <boost/property_tree/ptree.hpp>
#include "h5filehelper.h"
#include "h5outputfile.h"

using namespace MCPP;
%}


namespace boost {
  namespace random {}
  namespace property_tree {}
}

namespace H5 {}

typedef long long int u_int64_t;

%include "walker.h"
%include "baseobject.h"
%include "baserandom.h"
%include "distributions.h"
%include "costhetagenerator.h"
%include "psigenerator.h"
%include "source.h"
%include "gaussianraybundlesource.h"
%include "material.h"
%include "MCglobal.h"
%include "sample.h"
%include "simulation.h"
%include <boost/random.hpp>
%include <boost/property_tree/ptree.hpp>
%include "h5filehelper.h"
%include "h5outputfile.h"
