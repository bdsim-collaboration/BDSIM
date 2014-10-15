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

void TRKDecapole::Print(std::ostream &out)const {
  TRKElement::Print(out);
  out << "; Strength: " << strength << "T/m^4";
}
