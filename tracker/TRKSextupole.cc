#include "TRKSextupole.hh"
#include "TRKStrategy.hh"

/*
#include <cmath>
#include <complex>
#include <cstdlib>
#include "vector3.hh"
#include "vector6.hh"
*/

TRKSextupole::TRKSextupole(double strengthIn, std::string nameIn, double lengthIn, TRKAperture *apertureIn, TRKPlacement *placementIn):
  TRKElement(nameIn,lengthIn,apertureIn,placementIn), strength(strengthIn)
{
}

TRKSextupole::~TRKSextupole() {
}

void TRKSextupole::Track(TRKBunch* bunch, TRKStrategy* strategy) {
  strategy->Track(this,bunch);
}

std::ostream& operator<< (std::ostream &out, const TRKSextupole &element) {
  return out << element.strength << " ";
}
