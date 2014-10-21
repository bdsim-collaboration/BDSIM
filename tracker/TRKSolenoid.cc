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

void TRKSolenoid::Print(std::ostream &out)const {
  TRKElement::Print(out);
  out << "; Strength: " << strength << "T/m";
}
