#include "BDSGlobalConstants.hh"
#include "BDSOctMagField.hh"

#include "G4AffineTransform.hh"
#include "G4ThreeVector.hh"

BDSOctMagField::BDSOctMagField(G4double aBTrpPrime):
  itsBTrpPrime(aBTrpPrime)
{;}

BDSOctMagField::~BDSOctMagField()
{;}

void BDSOctMagField::GetFieldValue(const G4double Point[4],
				   G4double *Bfield ) const
{
  G4ThreeVector GlobalR(Point[0], Point[1], Point[2]);
  
  auxNavigator->LocateGlobalPointAndSetup(GlobalR);
  G4AffineTransform globalToLocal = auxNavigator->GetGlobalToLocalTransform();
  G4AffineTransform localToGlobal = auxNavigator->GetLocalToGlobalTransform();
  G4ThreeVector     LocalR        = globalToLocal.TransformPoint(GlobalR); 
  
  G4double bx = (3*LocalR.x()*LocalR.x()*LocalR.y()-pow(LocalR.y(),3))*itsBTrpPrime/6.;
  G4double by = (pow(LocalR.x(),3)-LocalR.x()*LocalR.y()*LocalR.y())*itsBTrpPrime/6.;
  // factor of 6 is actually 3-factorial.

  G4ThreeVector localB(bx, by, 0);

  G4ThreeVector globalB = localToGlobal.TransformAxis(localB);
  Bfield[0] = globalB[0];
  Bfield[1] = globalB[1];
  Bfield[2] = globalB[2];
}



