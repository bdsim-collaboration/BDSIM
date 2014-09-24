#include "TRKDipole.hh"

#include <cmath>
#include <cfloat>
#include <cstdlib>
#include <limits>
#include "vector3.hh"
#include "vector6.hh"

#include "CLHEP/Units/SystemOfUnits.h"

TRKDipole::TRKDipole(double strengthIn, double bFieldIn, std::string nameIn, double lengthIn, TRKAperture *apertureIn, TRKPlacement *placementIn):
  TRKElement(nameIn,lengthIn,apertureIn,placementIn), strength(strengthIn), bField(bFieldIn)
{
}

TRKDipole::~TRKDipole() {
}

std::ostream& operator<< (std::ostream &out, const TRKDipole &element) {
  return out << element.strength << " ";
}
