#include "BDSGlobalConstants.hh"
#include "BDSFieldMagOctupole.hh"

#include "G4AffineTransform.hh"
#include "G4ThreeVector.hh"

BDSFieldMagOctupole::BDSFieldMagOctupole(G4double aBTrpPrime):
  itsBTrpPrime(aBTrpPrime)
{;}

BDSFieldMagOctupole::~BDSFieldMagOctupole()
{;}

void BDSFieldMagOctupole::GetFieldValue(const G4double Point[4],
				   G4double *Bfield ) const
{
  G4ThreeVector GlobalR(Point[0], Point[1], Point[2]);
  
  auxNavigator->LocateGlobalPointAndSetup(GlobalR);
  G4AffineTransform GlobalAffine = auxNavigator->GetGlobalToLocalTransform();
  G4ThreeVector     LocalR       = GlobalAffine.TransformPoint(GlobalR); 
  
  Bfield[0] = (3*LocalR.x()*LocalR.x()*LocalR.y()-pow(LocalR.y(),3))*itsBTrpPrime/6.;
  Bfield[1] = (pow(LocalR.x(),3)-LocalR.x()*LocalR.y()*LocalR.y())*itsBTrpPrime/6.;
  Bfield[2] = 0;
  // factor of 6 is actually 3-factorial.
}



