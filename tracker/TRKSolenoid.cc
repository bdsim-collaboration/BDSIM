#include "TRKSolenoid.hh"
#include "TRKStrategy.hh"

TRKSolenoid::TRKSolenoid(double strengthIn, std::string nameIn, double lengthIn, TRKAperture *apertureIn, TRKPlacement *placementIn):
  TRKElement(nameIn,lengthIn,apertureIn,placementIn), strength(strengthIn)
{
}

TRKSolenoid::~TRKSolenoid() {
}

void TRKSolenoid::Track(TRKBunch* bunch, TRKStrategy* strategy) {
  strategy->Track(this,bunch);
}

std::ostream& operator<< (std::ostream &out, const TRKSolenoid &element) {
  return out << element.strength << " ";
}
