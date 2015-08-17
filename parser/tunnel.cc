#include "tunnel.h"

#include <iomanip>
#include <iostream>

using namespace GMAD;

Tunnel::Tunnel():
  aper1(0.0),aper2(0.0),
  offsetX(0.0),offsetY(0.0),
  thickness(0.0),soilThickness(0.0),
  floorOffset(0.0),visible(true)
{;}

void Tunnel::clear()
{
  name = "";
  type = "";
  
  aper1         = 0.0;
  aper2         = 0.0;
  offsetX       = 0.0;
  offsetY       = 0.0;
  thickness     = 0.0;
  soilThickness = 0.0;
  floorOffset   = 0.0;
  visible       = true;

  material     = "";
  soilMaterial = "";
  startElement = "";
  endElement   = "";
}

void Tunnel::print()const
{
  std::cout << "tunnel: "
	    << name          << " "
	    << type          << " "
	    << aper1         << " "
	    << aper2         << " "
	    << offsetX       << " "
	    << offsetY       << " "
	    << thickness     << " "
	    << soilThickness << " "
	    << floorOffset   << " "
	    << material      << " "
	    << soilMaterial  << " "
	    << startElement  << " "
	    << endElement
	    << std::endl;
}

void Tunnel::set_value(std::string property, double value )
{
#ifdef BDSDEBUG
  std::cout << "parser> Setting value " << std::setw(25) << std::left << property << value << std::endl;
#endif

  if (property=="radius" || property == "aper1") {aper1 = value; return;}
  if (property=="aper2")         {aper2 = value; return;}
  if (property=="offsetX")       {offsetX = value; return;}
  if (property=="offsetY")       {offsetY = value; return;}
  if (property=="thickness")     {thickness = value; return;}
  if (property=="soilThickness") {soilThickness = value; return;}
  if (property=="floorOffset")   {floorOffset = value; return;}
  if (property=="visible")       {visible = (bool)value; return;}
  
  std::cerr << "Error: parser> unknown tunnel option \"" << property << "\" with value " << value << std::endl; 
  exit(1);
}

void Tunnel::set_value(std::string property, std::string value )
{
  #ifdef BDSDEBUG
  std::cout << "parser> Setting value " << std::setw(25) << std::left << property << value << std::endl;
#endif

  if (property=="name")         {name = value; return;}
  if (property=="type")         {type = value; return;}
  if (property=="material")     {material = value; return;}
  if (property=="soilMaterial") {soilMaterial = value; return;}
  if (property=="startElement") {startElement = value; return;}
  if (property=="endElement")   {endElement = value; return;}

  std::cerr << "Error: parser> unknown tunnel option \"" << property << "\" with value " << value << std::endl;
  exit(1);
}
