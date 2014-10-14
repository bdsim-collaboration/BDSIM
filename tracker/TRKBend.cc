#include "TRKBend.hh"
#include "TRKStrategy.hh"

TRKBend::TRKBend(double strengthIn, double angleIn, std::string nameIn, double lengthIn, TRKAperture *apertureIn, TRKPlacement *placementIn):
  TRKElement(nameIn,lengthIn,apertureIn,placementIn), strength(strengthIn), angle(angleIn)
{
}

TRKBend::~TRKBend() {
}

void TRKBend::Track(TRKBunch* bunch, TRKStrategy* strategy) {
  strategy->Track(this,bunch);
}

std::ostream& operator<< (std::ostream &out, const TRKBend &element) {
  return out << element.strength << " " << element.angle << " ";
}
