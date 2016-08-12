#include "BDSGlobalConstants.hh" 
#include "BDSSextMagField.hh"

BDSSextMagField::BDSSextMagField(G4double aBDblePrime):
  itsBDblePrime(aBDblePrime)
{;}

BDSSextMagField::~BDSSextMagField()
{;}

void BDSSextMagField::GetFieldValue(const G4double Point[4],
				    G4double *Bfield ) const
{
  G4ThreeVector GlobalR(Point[0], Point[1], Point[2]);

  auxNavigator->LocateGlobalPointAndSetup(GlobalR);
  G4AffineTransform globalToLocal = auxNavigator->GetGlobalToLocalTransform();
  G4AffineTransform localToGlobal = auxNavigator->GetLocalToGlobalTransform();
  G4ThreeVector LocalR = globalToLocal.TransformPoint(GlobalR); 
  
  G4double bx = 2*LocalR.x()*LocalR.y()*itsBDblePrime/2;
  G4double by = (LocalR.x()*LocalR.x()-LocalR.y()*LocalR.y())*itsBDblePrime/2;
  // factor of 2 is actually 2-factorial.
  G4ThreeVector localB(bx, by, 0);

  G4ThreeVector globalB = localToGlobal.TransformAxis(localB);
  Bfield[0] = globalB[0];
  Bfield[1] = globalB[1];
  Bfield[2] = globalB[2];
}



