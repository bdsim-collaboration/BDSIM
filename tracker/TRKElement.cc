#include "TRKElement.hh"

TRKElement::TRKElement() {} 

TRKElement::TRKElement(std::string nameIn, double lengthIn, double size_xIn, double size_yIn,
		       TRKAperture *apertureIn, TRKPlacement *placementIn) :
  name(nameIn), length(lengthIn), size_x(size_xIn), size_y(size_yIn), aperture(apertureIn), placement(placementIn)
{} 

TRKElement::~TRKElement() {}
