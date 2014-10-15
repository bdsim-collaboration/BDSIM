#include "TRKDecapole.hh"
#include "TRKStrategy.hh"

TRKDecapole::TRKDecapole(double strengthIn, std::string nameIn, double lengthIn, TRKAperture *apertureIn, TRKPlacement *placementIn):
  TRKElement(nameIn,lengthIn,apertureIn,placementIn), strength(strengthIn)
{
}

TRKDecapole::~TRKDecapole() {
}

void TRKDecapole::Track(TRKBunch* bunch, TRKStrategy* strategy) {
  strategy->Track(this,bunch);
}

std::ostream& operator<< (std::ostream &out, const TRKDecapole &element) {
  return out << element.strength << " ";
}
