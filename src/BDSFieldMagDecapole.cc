#include "BDSFieldMagDecapole.hh"

#include "globals.hh" // geant4 types / globals
#include "G4AffineTransform.hh"

BDSFieldMagDecapole::BDSFieldMagDecapole(G4double aBQuadPrime):
  itsBQuadPrime(aBQuadPrime)
{;}
 
BDSFieldMagDecapole::~BDSFieldMagDecapole()
{;}

void BDSFieldMagDecapole::GetFieldValue(const G4double Point[4],
				   G4double *Bfield) const
{
  G4ThreeVector     GlobalR      = G4ThreeVector(Point[0], Point[1], Point[2]);
  G4AffineTransform GlobalAffine = auxNavigator->GetGlobalToLocalTransform();
  G4ThreeVector     LocalR       = GlobalAffine.TransformPoint(GlobalR); 

  Bfield[0]= 4*LocalR.x()*LocalR.y()*(LocalR.x()*LocalR.x()
				      -LocalR.y()*LocalR.y())*itsBQuadPrime/24;
  Bfield[1]=(pow(LocalR.x(),4)-6*LocalR.x()*LocalR.x()*LocalR.y()*LocalR.y()+
	     pow(LocalR.y(),4))*itsBQuadPrime/24;
  Bfield[2]=0;
  // factor of 24 is actually 4-factorial.
}



