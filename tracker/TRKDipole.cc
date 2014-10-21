#include "TRKDipole.hh"
#include "TRKStrategy.hh"
/*
#include <cmath>
#include <cfloat>
#include <cstdlib>
#include <limits>
#include "vector3.hh"
#include "vector6.hh"

#include "CLHEP/Units/SystemOfUnits.h"
*/

TRKDipole::TRKDipole(double strengthIn, double bFieldIn, std::string nameIn, double lengthIn, TRKAperture *apertureIn, TRKPlacement *placementIn):
  TRKElement(nameIn,lengthIn,apertureIn,placementIn), strength(strengthIn), bField(bFieldIn)
{
  // TODO - calculate integrated strength from length and particle type
  intStrength = strengthIn;
}

TRKDipole::~TRKDipole() {
}

void TRKDipole::Track(TRKBunch* bunch, TRKStrategy* strategy) {
  strategy->Track(this,bunch);
}

void TRKDipole::Print(std::ostream &out)const {
  TRKElement::Print(out);
  out << "; Strength: " << strength << "T";
}
