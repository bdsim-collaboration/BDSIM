#include "tunnel.h"

using namespace GMAD;

Tunnel::Tunnel():
  aper1(0.0),aper2(0.0),
  offsetX(0.0),offsetY(0.0),
  thickness(0.0),soilThickness(0.0),
  floorOffset(0.0),visible(true)
{
  PublishMembers();
}

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

void Tunnel::PublishMembers()
{
  publish("name",&Tunnel::name);
  publish("type",&Tunnel::type);
  publish("radius",&Tunnel::aper1);
  publish("aper1",&Tunnel::aper1);
  publish("aper2",&Tunnel::aper2);
  publish("offsetX",&Tunnel::offsetX);
  publish("offsetY",&Tunnel::offsetY);
  publish("thickness",&Tunnel::thickness);
  publish("soilThickness",&Tunnel::soilThickness);
  publish("floorOffset",&Tunnel::floorOffset);
  publish("visible",&Tunnel::visible);
  publish("material",&Tunnel::material);
  publish("soilMaterial",&Tunnel::soilMaterial);
  publish("startElement",&Tunnel::startElement);
  publish("endElement",&Tunnel::endElement);
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
