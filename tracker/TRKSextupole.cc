#include "TRKSextupole.hh"

#include <cmath>
#include <complex>
#include <cstdlib>
#include "vector3.hh"
#include "vector6.hh"

TRKSextupole::TRKSextupole(double strengthIn, std::string nameIn, double lengthIn, TRKAperture *apertureIn, TRKPlacement *placementIn):
  TRKElement(nameIn,lengthIn,apertureIn,placementIn), strength(strengthIn)
{
}

TRKSextupole::~TRKSextupole() {
}

std::ostream& operator<< (std::ostream &out, const TRKSextupole &element) {
  return out << element.strength << " ";
}
