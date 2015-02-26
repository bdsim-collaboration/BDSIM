#include "TRKRBend.hh"
#include "TRKStrategy.hh"

TRKRBend::TRKRBend(double angleIn, std::string nameIn, double lengthIn, TRKAperture *apertureIn, TRKPlacement *placementIn):
  TRKElement(nameIn,lengthIn,apertureIn,placementIn), angle(angleIn)
{
}

TRKRBend::~TRKRBend() {
}

void TRKRBend::Track(TRKBunch* bunch, TRKStrategy* strategy) {
  strategy->Track(this,bunch);
}

void TRKRBend::Print(std::ostream &out)const {
  TRKElement::Print(out);
  out << "; Angle: " << angle << "rad";
}
