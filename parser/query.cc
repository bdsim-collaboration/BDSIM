#include "query.h"

using namespace GMAD;

Query::Query()
{
  clear();
  PublishMembers();
}

void Query::clear()
{
  name        = "";
  fieldObject = "";
  nx   = 1;
  ny   = 1;
  nz   = 1;
  nt   = 1;
  xmin = 0;
  xmax = 1;
  ymin = 0;
  ymax = 1;
  zmin = 0;
  zmax = 1;
  tmin = 0;
  tmax = 0;
  outfile = "";
}

void Query::PublishMembers()
{
  publish("name",        &Query::name);
  publish("fieldObject", &Query::fieldObject);
  publish("nx",          &Query::nx);
  publish("ny",          &Query::ny);
  publish("nz",          &Query::nz);
  publish("nt",          &Query::nt);
  publish("xmin",        &Query::xmin);
  publish("ymin",        &Query::ymin);
  publish("zmin",        &Query::zmin);
  publish("tmin",        &Query::tmin);
  publish("xmax",        &Query::xmax);
  publish("ymax",        &Query::ymax);
  publish("zmax",        &Query::zmax);
  publish("tmax",        &Query::tmax);
  publish("outfile",     &Query::outfile);
}

void Query::print()const
{
  std::cout << "query: "
	    << "name "        << name        << std::endl
	    << "fieldObject " << fieldObject << std::endl
	    << "nx "  << nx   << std::endl
    	    << "ny "  << ny   << std::endl
    	    << "nz "  << nz   << std::endl
    	    << "nt "  << ny   << std::endl
	    << "x: (" << xmin << ", " << xmax << ")" << std::endl
    	    << "y: (" << ymin << ", " << ymax << ")" << std::endl
    	    << "z: (" << zmin << ", " << zmax << ")" << std::endl
    	    << "t: (" << tmin << ", " << tmax << ")" << std::endl;
}
