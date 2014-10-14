#include "TRKDrift.hh"
#include "TRKStrategy.hh"


TRKDrift::TRKDrift(std::string nameIn, double lengthIn, TRKAperture *apertureIn, TRKPlacement *placementIn):
  TRKElement(nameIn,lengthIn,apertureIn,placementIn)
{}

TRKDrift::~TRKDrift() 
{}

void TRKDrift::Track(TRKBunch* bunch, TRKStrategy* strategy) {
  strategy->Track(this,bunch);
}

std::ostream& operator<< (std::ostream &out, const TRKDrift &element) {
  return out << element.name << " ";
}
