#include "BDSDecMagField.hh"
#include "BDSGlobalConstants.hh" 

BDSDecMagField::BDSDecMagField(G4double aBQuadPrime):
  itsBQuadPrime(aBQuadPrime)
{;}
 
BDSDecMagField::~BDSDecMagField()
{;}

void BDSDecMagField::GetFieldValue( const G4double Point[4],
		       G4double *Bfield ) const
{
  G4ThreeVector LocalR, GlobalR;

  GlobalR.setX(Point[0]);
  GlobalR.setY(Point[1]);
  GlobalR.setZ(Point[2]);

  G4AffineTransform GlobalAffine = auxNavigator->GetGlobalToLocalTransform();
  LocalR = GlobalAffine.TransformPoint(GlobalR); 

  Bfield[0]= 4*LocalR.x()*LocalR.y()*(LocalR.x()*LocalR.x()
				      -LocalR.y()*LocalR.y())*itsBQuadPrime/24;
  Bfield[1]=(pow(LocalR.x(),4)-6*LocalR.x()*LocalR.x()*LocalR.y()*LocalR.y()+
	     pow(LocalR.y(),4))*itsBQuadPrime/24;
  Bfield[2]=0;

  // factor of 24 is actually 4-factorial.
}



