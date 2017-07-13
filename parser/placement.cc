#include "placement.h"

using namespace GMAD;

Placement::Placement()
{
  clear();
  PublishMembers();
}

void Placement::clear()
{
  name         = "";
  geometryFile = "";
  sequence     = "";
  referenceElement = "";
  s     = 0;
  x     = 0;
  y     = 0;
  z     = 0;
  phi   = 0;
  theta = 0;
  psi   = 0;
  axisX = 0;
  axisY = 0;
  axisZ = 0;
  angle = 0;
  sensitive     = true;
  axisAngle     = false;
}

void Placement::PublishMembers()
{
  publish("name",          &Placement::name);
  publish("geometryFile",  &Placement::geometryFile);
  publish("sequence",      &Placement::sequence);
  publish("referencElement", &Placement::referenceElement);
  publish("s",             &Placement::s);
  publish("x",             &Placement::x);
  publish("y",             &Placement::y);
  publish("z",             &Placement::z);
  publish("phi",           &Placement::phi);
  publish("theta",         &Placement::theta);
  publish("psi",           &Placement::psi);
  publish("axisX",         &Placement::axisX);
  publish("axisY",         &Placement::axisY);
  publish("axisZ",         &Placement::axisZ);
  publish("angle",         &Placement::angle);
  publish("sensitive",     &Placement::sensitive);
  publish("axisAngle",     &Placement::axisAngle);
}

void Placement::print()const
{
  std::cout << "Placement: "
	    << "name "          << name          << std::endl
	    << "geometryFile "  << geometryFile  << std::endl
	    << "sequence "      << sequence      << std::endl
	    << "referenceElement" << referenceElement << std::endl
	    << "s"              << s             << std::endl
	    << "x "             << x             << std::endl
    	    << "y "             << y             << std::endl
    	    << "z "             << z             << std::endl
    	    << "phi "           << phi           << std::endl
    	    << "theta "         << theta         << std::endl
    	    << "psi "           << psi           << std::endl
    	    << "axisX "         << axisX         << std::endl
    	    << "axisY "         << axisY         << std::endl
    	    << "axisZ "         << axisZ         << std::endl
    	    << "angle "         << angle         << std::endl
	    << "sensitive "     << sensitive     << std::endl
	    << "axisAngle "     << axisAngle     << std::endl;
}
