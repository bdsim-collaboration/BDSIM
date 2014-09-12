#include "TRKElement.hh"

TRKElement::TRKElement(std::string nameIn, double lengthIn,
		       TRKAperture *apertureIn, TRKPlacement *placementIn) :
  name(nameIn), length(lengthIn), aperture(apertureIn), placement(placementIn)
{} 

TRKElement::~TRKElement() {}

std::ostream& operator<< (std::ostream &out, const TRKElement &element) {
  return out << element.name << " "
	     << element.length << "m ";
}
