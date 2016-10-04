#include "cavitymodel.h"

using namespace GMAD;

CavityModel::CavityModel()
{
  clear();
  PublishMembers();
}

void CavityModel::clear()
{
  name                   = "";
  type                   = "";
  irisRadius             = 0.0;
  equatorRadius          = 0.0;
  halfCellLength         = 0.0;
  equatorEllipseSemiAxis = 0.0;
  irisHorizontalAxis     = 0.0;
  irisVerticalAxis       = 0.0;
  tangentLineAngle       = 0.0;
  thickness              = 0.0;
  frequency              = 0.0;
  phase                  = 0.0;
  eField                 = 0.0;
  numberOfPoints         = 24;
  numberOfCells          = 1;
}

void CavityModel::PublishMembers()
{
  publish("name",               &CavityModel::name);
  publish("type",               &CavityModel::type);
  publish("irisRadius",         &CavityModel::irisRadius);
  publish("halfCellLength",     &CavityModel::halfCellLength);
  publish("equatorEllipseSemiAxis",&CavityModel::equatorEllipseSemiAxis);
  publish("equatorRadius",      &CavityModel::equatorRadius);
  publish("irisHorizontalAxis", &CavityModel::irisHorizontalAxis);
  publish("irisVerticalAxis",   &CavityModel::irisVerticalAxis);
  publish("tangentLineAngle",   &CavityModel::tangentLineAngle);
  publish("thickness",          &CavityModel::thickness);
  publish("frequency",          &CavityModel::frequency);
  publish("phase",              &CavityModel::phase);
  publish("efield",             &CavityModel::eField);
  publish("numberOfPoints",     &CavityModel::numberOfPoints);
  publish("numberOfCells",      &CavityModel::numberOfCells);
}

void CavityModel::print()const
{
  std::cout << "cavitymodel: "
	    << "name "                   << name << " " << std::endl
	    << "type "                   << type << " " << std::endl
	    << "irisRadius "             << irisRadius << " " << std::endl
	    << "equatorRadius "          << equatorRadius << " " << std::endl
	    << "halfCellLength "         << halfCellLength << " " << std::endl
	    << "equatorEllipseSemiAxis " << equatorEllipseSemiAxis << " " << std::endl
	    << "irisHorizontalAxis "     << irisHorizontalAxis << " " << std::endl
	    << "irisVerticalAxis "       << irisVerticalAxis << " " << std::endl
	    << "tangentLineAngle "       << tangentLineAngle << " " << std::endl
	    << "thickness "              << thickness << " " << std::endl
	    << "frequency "              << frequency << " " << std::endl
	    << "phase "                  << phase << " " << std::endl
	    << "eField "                 << eField << " " << std::endl
	    << "numberOfPoints "         << numberOfPoints << " " << std::endl
	    << "numberOfCells "          << numberOfCells << " " << std::endl
	    << std::endl;
}
