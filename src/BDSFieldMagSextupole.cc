#include "BDSGlobalConstants.hh" 
#include "BDSFieldMagSextupole.hh"

BDSFieldMagSextupole::BDSFieldMagSextupole(G4double aBDblePrime):
  itsBDblePrime(aBDblePrime)
{;}

BDSFieldMagSextupole::~BDSFieldMagSextupole()
{;}

void BDSFieldMagSextupole::GetFieldValue(const G4double Point[4],
				    G4double *Bfield ) const
{
  G4ThreeVector GlobalR(Point[0], Point[1], Point[2]);

  auxNavigator->LocateGlobalPointAndSetup(GlobalR);
  G4AffineTransform GlobalAffine = auxNavigator->GetGlobalToLocalTransform();
  G4ThreeVector LocalR = GlobalAffine.TransformPoint(GlobalR); 

  Bfield[0]=2*LocalR.x()*LocalR.y()*itsBDblePrime/2;
  Bfield[1]=(LocalR.x()*LocalR.x()-LocalR.y()*LocalR.y())*itsBDblePrime/2;
  Bfield[2]=0;

  // factor of 2 is actually 2-factorial.
}



