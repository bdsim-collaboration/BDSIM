#include "BDSGlobalConstants.hh" 
#include "BDSFieldMagQuadrupole.hh"

#include "G4AffineTransform.hh"
#include "G4ThreeVector.hh"

BDSFieldMagQuadrupole::BDSFieldMagQuadrupole(G4double aBGrad):
  itsBGrad(aBGrad)
{;}

BDSFieldMagQuadrupole::~BDSFieldMagQuadrupole()
{;}

void BDSFieldMagQuadrupole::GetFieldValue( const G4double Point[4],
		       G4double *Bfield ) const
{
  G4ThreeVector GlobalR(Point[0], Point[1], Point[2]);
  
  auxNavigator->LocateGlobalPointAndSetup(GlobalR);
  G4AffineTransform GlobalAffine = auxNavigator->GetGlobalToLocalTransform();
  G4ThreeVector     LocalR       = GlobalAffine.TransformPoint(GlobalR); 
  
  Bfield[0] = LocalR.y()*itsBGrad;
  Bfield[1] = LocalR.x()*itsBGrad;
  Bfield[2] = 0;
  // factor of 2 is actually 2-factorial.
}



