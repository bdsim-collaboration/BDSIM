#include "TRKElement.hh"

#include "TRKAperture.hh"
#include "TRKPlacement.hh"
#include "TRKStrategy.hh"
#include "TRKTiltOffset.hh"

TRKElement::TRKElement(std::string nameIn, double lengthIn,
		       TRKAperture *apertureIn, TRKPlacement *placementIn) :
  name(nameIn), length(lengthIn), aperture(apertureIn), placement(placementIn)
{
  offsetIn  = NULL;
  offsetOut = NULL;
} 

TRKElement::~TRKElement() {
  delete aperture;
  delete placement;
  delete offsetIn;
  delete offsetOut;
}


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

void TRKElement::ConfirmCoordinates(TRKBunch* bunch)
{
  //in case we don't use aperture, we still have to confirm coordinates
  TRKBunchIter iter = bunch->begin();
  TRKBunchIter iend = bunch->end();
  for (;iter != iend; ++iter) {
    iter->ConfirmNewCoordinates();
  }
}

void TRKElement::SetOffset(double x, double y)
{
  if (!offsetIn) {
    offsetIn  = TRKTiltOffset::TRKOffset(x,y);
    offsetOut = TRKTiltOffset::TRKOffset(-x,-y);
  } else {
    offsetIn->SetOffset(x,y);
    offsetIn->SetOffset(-x,-y);
  }
}

void TRKElement::SetTilt(double phi, double theta, double psi)
{
  if (!offsetIn) {
    offsetIn  = TRKTiltOffset::TRKTilt(phi,theta,psi);
    offsetOut = TRKTiltOffset::TRKTilt(-phi,-theta,-psi);
  } else {
    offsetIn->SetTilt(phi,theta,psi);
    offsetOut->SetTilt(-phi,-theta,-psi);
  }
}

void TRKElement::AddOffset(double x, double y)
{
  if (offsetIn) {
    offsetIn->AddOffset(x,y);
    offsetOut->AddOffset(-x,-y);
  } else {
    SetOffset(x,y);
  }
}

void TRKElement::AddTilt(double phi, double theta, double psi)
{
  if (offsetIn) {
    offsetIn->AddTilt(phi,theta,psi);
    offsetOut->AddTilt(-phi,-theta,-psi);
  } else {
    SetTilt(phi,theta,psi);
  }
}

void TRKElement::Print(std::ostream &out)const {
  out << name << "; L= " << length << "m";
  if (aperture)  {out << "; Aperture: " << *(aperture);}
  if (placement) {out << "; Placement: " << *(placement);}
}
