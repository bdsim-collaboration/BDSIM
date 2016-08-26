#include "BDSGlobalConstants.hh" 
#include "BDSQuadMagField.hh"

#include "G4AffineTransform.hh"
#include "G4ThreeVector.hh"

BDSQuadMagField::BDSQuadMagField(G4double aBGrad):
  itsBGrad(aBGrad)
{;}

BDSQuadMagField::~BDSQuadMagField()
{;}

void BDSQuadMagField::GetFieldValue( const G4double Point[4],
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



