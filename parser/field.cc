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
  x         = 0;
  y         = 0;
  z         = 0;
  phi       = 0;
  theta     = 0;
  psi       = 0;
  axisX     = 0;
  axisY     = 0;
  axisZ     = 0;
  angle     = 0;
  axisAngle = false;
  scaling   = 1.0;
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
  publish("x",                    &Field::x);
  publish("y",                    &Field::y);
  publish("z",                    &Field::z);
  publish("phi",                  &Field::phi);
  publish("theta",                &Field::theta);
  publish("psi",                  &Field::psi);
  publish("axisX",                &Field::axisX);
  publish("axisY",                &Field::axisY);
  publish("axisZ",                &Field::axisZ);
  publish("angle",                &Field::angle);
  publish("axisAngle",            &Field::axisAngle);
  publish("scaling",              &Field::scaling);
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
	    << "electricInterpolator " << electricInterpolator << std::endl
	    << "x, y, z "              << x     << " " << y     << " " << z     << std::endl
	    << "phi, theta, psi "      << phi   << " " << theta << " " << psi   << std::endl
	    << "axisX, Y, Z "          << axisX << " " << axisY << " " << axisZ << std::endl
	    << "angle "                << angle                << std::endl
	    << "axisAngle "            << axisAngle            << std::endl
	    << "scaling "              << scaling              << std::endl;
}
