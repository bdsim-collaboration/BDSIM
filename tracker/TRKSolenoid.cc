#include "TRKSolenoid.hh"
#include "TRKStrategy.hh"
#include "TRKTiltOffset.hh"
#include "TRKBunch.hh"

TRKSolenoid::TRKSolenoid(double strengthIn, std::string nameIn, double lengthIn, TRKAperture *apertureIn, TRKPlacement *placementIn):
  TRKElement(nameIn,lengthIn,apertureIn,placementIn), strength(strengthIn)
{
}

TRKSolenoid::~TRKSolenoid() {
}

void TRKSolenoid::Track(TRKBunch* bunch, TRKStrategy* strategy) {
#ifdef TRKDEBUG
  std::cout << "Tracking through element named: " << name << std::endl;
  std::cout << "before tracking" << std::endl;
  std::cout << *bunch;
#endif

  if (offsetIn && offsetOut) strategy->Track(offsetIn,bunch);

  strategy->Track(this,bunch);

  if (offsetIn && offsetOut) strategy->Track(offsetOut,bunch);

#ifdef TRKDEBUG
  std::cout << "after tracking" << std::endl;
  std::cout << *bunch;
#endif
}

void TRKSolenoid::Print(std::ostream &out)const {
  TRKElement::Print(out);
  out << "; Strength: " << strength << "T/m";
}
