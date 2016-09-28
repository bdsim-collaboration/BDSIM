#include "field.h"

using namespace GMAD;

Field::Field()
{
  clear();
  PublishMembers();
}

void Field::clear()
{
  name                 = "";
  type                 = "";
  scaling              = 1.0;
  integrator           = "g4classicalrk4";
  globalTransform      = true;
  magneticFile         = "";
  magneticInterpolator = "nearest";
  electricFile         = "";
  electricInterpolator = "";
}

void Field::PublishMembers()
{
  publish("name",                 &Field::name);
  publish("type",                 &Field::type);
  publish("scaling",              &Field::scaling);
  publish("integrator",           &Field::integrator);
  publish("magneticFile",         &Field::magneticFile);
  publish("magneticInterpolator", &Field::magneticInterpolator);
  publish("electricFile",         &Field::electricFile);
  publish("electricInterpolator", &Field::electricInterpolator);
}

void Field::print()const
{
  std::cout << "field: "
	    << "name "                 << name                 << std::endl
	    << "type "                 << type                 << std::endl
	    << "scaling "              << scaling              << std::endl
	    << "integrator "           << integrator           << std::endl
	    << "magneticFile "         << magneticFile         << std::endl
	    << "magneticInterpolator " << magneticInterpolator << std::endl
	    << "electricFile "         << electricFile         << std::endl
	    << "electricInterpolator " << electricInterpolator << std::endl;
}
