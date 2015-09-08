#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
#include "BDSSolenoidMagField.hh"

BDSSolenoidMagField::BDSSolenoidMagField(G4double aBField):
  itsBField(aBField)
{;}

BDSSolenoidMagField::~BDSSolenoidMagField()
{;}

void BDSSolenoidMagField::GetFieldValue(const G4double* /*Point[4]*/,
					G4double *Bfield ) const
{
  // commented out because solenoid field (uniform) does not depend on position
  //G4ThreeVector GlobalR(Point[0], Point[1], Point[2]);
  //G4AffineTransform GlobalAffine = Navigator->GetGlobalToLocalTransform();
  //G4ThreeVector LocalR = GlobalAffine.TransformPoint(GlobalR); 

  G4ThreeVector LocalBField = G4ThreeVector(0.0, 0.0, itsBField);
  
  G4AffineTransform LocalAffine = auxNavigator->GetLocalToGlobalTransform();
  G4ThreeVector GlobalBField = LocalAffine.TransformAxis(LocalBField);

  Bfield[0]=GlobalBField.x();
  Bfield[1]=GlobalBField.y();
  Bfield[2]=GlobalBField.z();

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " B field = "
	 << Bfield[0] << " "
	 << Bfield[1] << " "
	 << Bfield[2] << G4endl;
#endif
}
