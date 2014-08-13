#include "TRKQuadrupole.hh"

TRKQuadrupole::TRKQuadrupole() {}

TRKQuadrupole::TRKQuadrupole(double strengthIn, TRKTrackingElement::TRKType typeIn, int trackingStepsIn, std::string nameIn, double lengthIn, double size_xIn, double size_yIn, TRKAperture *apertureIn, TRKPlacement *placementIn):
  TRKTrackingElement(typeIn, trackingStepsIn,nameIn,lengthIn,size_xIn,size_yIn,apertureIn,placementIn), strength(strengthIn)
{  
}

TRKQuadrupole::~TRKQuadrupole() {}


