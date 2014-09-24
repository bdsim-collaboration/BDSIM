#include "TRKSolenoid.hh"

TRKSolenoid::TRKSolenoid(double strengthIn, std::string nameIn, double lengthIn, TRKAperture *apertureIn, TRKPlacement *placementIn):
  TRKElement(nameIn,lengthIn,apertureIn,placementIn), strength(strengthIn)
{
}

TRKSolenoid::~TRKSolenoid() {
}

std::ostream& operator<< (std::ostream &out, const TRKSolenoid &element) {
  return out << element.strength << " ";
}
