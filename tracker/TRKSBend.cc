#include "TRKSBend.hh"
#include "TRKStrategy.hh"

TRKSBend::TRKSBend(double angleIn, std::string nameIn, double lengthIn, TRKAperture *apertureIn, TRKPlacement *placementIn):
  TRKElement(nameIn,lengthIn,apertureIn,placementIn), angle(angleIn)
{
}

TRKSBend::~TRKSBend() {
}

void TRKSBend::Track(TRKBunch* bunch, TRKStrategy* strategy) {
  strategy->Track(this,bunch);
}

void TRKSBend::Print(std::ostream &out)const {
  TRKElement::Print(out);
  out << "; Angle: " << angle << "rad";
}
