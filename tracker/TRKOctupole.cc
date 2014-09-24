#include "TRKOctupole.hh"

#include <cmath>
#include <complex>
#include <cstdlib>
#include "vector3.hh"
#include "vector6.hh"

TRKOctupole::TRKOctupole(double strengthIn, std::string nameIn, double lengthIn, TRKAperture *apertureIn, TRKPlacement *placementIn):
  TRKElement(nameIn,lengthIn,apertureIn,placementIn), strength(strengthIn)
{
}

TRKOctupole::~TRKOctupole() {
}

std::ostream& operator<< (std::ostream &out, const TRKOctupole &element) {
  return out << element.strength << " ";
}
