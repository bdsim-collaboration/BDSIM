#include "TRKBend.hh"

TRKBend::TRKBend(double strengthIn, double angleIn, std::string nameIn, double lengthIn, TRKAperture *apertureIn, TRKPlacement *placementIn):
  TRKElement(nameIn,lengthIn,apertureIn,placementIn), strength(strengthIn), angle(angleIn)
{
}

TRKBend::~TRKBend() {
}

std::ostream& operator<< (std::ostream &out, const TRKBend &element) {
  return out << element.strength << " " << element.angle << " ";
}
