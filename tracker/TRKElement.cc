/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2017.

This file is part of BDSIM.

BDSIM is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published 
by the Free Software Foundation version 3 of the License.

BDSIM is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BDSIM.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "TRKElement.hh"

#include "TRKAperture.hh"
#include "TRKPlacement.hh"
#include "TRKStrategy.hh"
#include "TRKTiltOffset.hh"

TRKElement::TRKElement(std::string  nameIn,
		       double       lengthIn,
		       TRKAperture *apertureIn,
		       TRKPlacement *placementIn,
		       double        strengthIn):
  name(nameIn),
  length(lengthIn),
  strength(strengthIn),
  aperture(apertureIn),
  placement(placementIn),
  sample(false)
{
  offsetIn  = nullptr;
  offsetOut = nullptr;
} 

TRKElement::~TRKElement()
{
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

TRKBunch* TRKElement::CheckAperture(TRKBunch* bunch)
{
  //use member instance of Aperture strategy called aperture
  if (aperture) 
    {return aperture->CheckAperture(bunch);}
  else
    {return nullptr;}
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
