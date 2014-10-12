#include "TRKElement.hh"

#include "TRKStrategy.hh"

TRKElement::TRKElement(std::string nameIn, double lengthIn,
		       TRKAperture *apertureIn, TRKPlacement *placementIn) :
  name(nameIn), length(lengthIn), aperture(apertureIn), placement(placementIn)
{} 

TRKElement::~TRKElement() {}

void TRKElement::Track(TRKBunch* bunch, TRKStrategy* strategy)
{
#ifdef TRKDEBUG
  std::cout << "before tracking" << std::endl;
  std::cout << *bunch;
#endif

  strategy->Track(this,bunch);

#ifdef TRKDEBUG
  std::cout << "after tracking" << std::endl;
  std::cout << *bunch;
#endif
}

void TRKElement::CheckAperture(TRKBunch* bunch)
{
  //use member instance of Aperture strategy called aperture
  if (aperture) 
    {aperture->CheckAperture(bunch);}
}

std::ostream& operator<< (std::ostream &out, const TRKElement &element) {
  out << element.name << "; L= "
      << element.length << "m; Aperture: ";
  if (element.aperture)  {out << *(element.aperture);}
  if (element.placement) {out << *(element.placement);}
  return out;
}
