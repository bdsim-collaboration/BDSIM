#include "TRKElement.hh"

#include "TRKStrategy.hh"

TRKElement::TRKElement(std::string nameIn, double lengthIn,
		       TRKAperture *apertureIn, TRKPlacement *placementIn) :
  name(nameIn), length(lengthIn), aperture(apertureIn), placement(placementIn)
{
  offsetIn  = NULL;
  offsetOut = NULL;
} 

TRKElement::~TRKElement() {}


void TRKElement::Track(TRKBunch* bunch, TRKStrategy* strategy)
{
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

void TRKElement::CheckAperture(TRKBunch* bunch)
{
  //use member instance of Aperture strategy called aperture
  if (aperture) 
    {aperture->CheckAperture(bunch);}
}

void TRKElement::Print(std::ostream &out)const {
  out << name << "; L= " << length << "m";
  if (aperture)  {out << "; Aperture: " << *(aperture);}
  if (placement) {out << "; Placement: " << *(placement);}
}
