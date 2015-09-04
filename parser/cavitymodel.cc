#include "cavitymodel.h"

#include <iomanip>
#include <iostream>

using namespace GMAD;

CavityModel::CavityModel()
{
  clear();
}

void CavityModel::clear()
{
  name = "";
  type = "";
  irisRadius = 0.0;
  equatorRadius = 0.0;
  halfCellLength = 0.0;
  equatorEllipseSemiAxis = 0.0;
  irisHorizontalAxis = 0.0;
  irisVerticalAxis = 0.0;
  tangentLineAngle = 0.0;
  thickness = 0.0;
  numberOfPoints = 24;
  numberOfCells = 1;
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
	    << "numberOfPoints "         << numberOfPoints << " " << std::endl
	    << "numberOfCells "          << numberOfCells << " " << std::endl
	    << std::endl;
}

void CavityModel::set_value(std::string property, double value )
{
#ifdef BDSDEBUG
  std::cout << "parser> Setting value " << std::setw(25) << std::left << property << value << std::endl;
#endif

  if (property=="irisRadius") {irisRadius = value; return;}
  if (property=="equatorRadius") {equatorRadius = value; return;}
  if (property=="halfCellLength") {halfCellLength = value; return;}
  if (property=="equatorEllipseSemiAxis") {equatorEllipseSemiAxis = value; return;}
  if (property=="irisHorizontalAxis") {irisHorizontalAxis = value; return;}
  if (property=="irisVerticalAxis") {irisVerticalAxis = value; return;}
  if (property=="tangentLineAngle") {tangentLineAngle = value; return;}
  if (property=="thickness") {thickness = value; return;}
  if (property=="numberOfPoints") {numberOfPoints = value; return;}
  if (property=="numberOfCells") {numberOfCells = value; return;}

  std::cerr << "Error: parser> unknown cavitymodel option \"" << property << "\" with value " << value << std::endl; 
  exit(1);
}

void CavityModel::set_value(std::string property, std::string value )
{
#ifdef BDSDEBUG
  std::cout << "parser> Setting value " << std::setw(25) << std::left << property << value << std::endl;
#endif

  if (property=="name")         {name = value; return;}
  if (property=="type")         {type = value; return;}

  std::cerr << "Error: parser> unknown cavitymodel option \"" << property << "\" with value " << value << std::endl;
  exit(1);
}
