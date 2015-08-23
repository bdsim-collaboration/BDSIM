#include "BDSGlobalConstants.hh" 
#include "BDSMuSpoilerMagField.hh"

#include "globals.hh"
#include "G4AffineTransform.hh"
#include "G4ThreeVector.hh"

#include <cmath>

BDSMuSpoilerMagField::BDSMuSpoilerMagField(G4double aField):
  itsBField(aField)
{;}

BDSMuSpoilerMagField::~BDSMuSpoilerMagField()
{;}

void BDSMuSpoilerMagField::GetFieldValue(const G4double Point[4],
					      G4double *Bfield ) const
{
  G4ThreeVector GlobalPosition = G4ThreeVector(Point[0], Point[1], Point[2]);  
  auxNavigator->LocateGlobalPointAndSetup(GlobalPosition);

  G4AffineTransform GlobalAffine = auxNavigator->GetGlobalToLocalTransform();
  G4ThreeVector LocalR = GlobalAffine.TransformPoint(GlobalPosition); 

  G4double LocalX = LocalR.x();
  G4double LocalY = LocalR.y();
  G4double LocalRadius = std::sqrt(LocalX*LocalX + LocalY*LocalY);

  Bfield[0]=  LocalY/LocalRadius * itsBField;
  Bfield[1]= -LocalX/LocalRadius * itsBField;
  Bfield[2]=0;
}



