#include "TRKDecapole.hh"

TRKDecapole::TRKDecapole(double strengthIn, std::string nameIn, double lengthIn, TRKAperture *apertureIn, TRKPlacement *placementIn):
  TRKElement(nameIn,lengthIn,apertureIn,placementIn), strength(strengthIn)
{
}

TRKDecapole::~TRKDecapole() {
}

std::ostream& operator<< (std::ostream &out, const TRKDecapole &element) {
  return out << element.strength << " ";
}
