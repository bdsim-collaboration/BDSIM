#include "TRKElement.hh"

#include "TRKStrategy.hh"

TRKElement::TRKElement(std::string nameIn, double lengthIn,
		       TRKAperture *apertureIn, TRKPlacement *placementIn) :
  name(nameIn), length(lengthIn), aperture(apertureIn), placement(placementIn)
{} 

TRKElement::~TRKElement() {}

void TRKElement::Track(TRKBunch* bunch, TRKStrategy* strategy)
{
  strategy->Track(this,bunch);
}
/*
void TRKElement::CheckAperture(TRKBunch* bunch);

, TRKApertureStrategy* this->aperture)
{
//use member instance of Aperture strategy called aperture
  aperture->CheckAperture(bunch);
}
*/
std::ostream& operator<< (std::ostream &out, const TRKElement &element) {
  return out << element.name << " "
	     << element.length << "m ";
}
