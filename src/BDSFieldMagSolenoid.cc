#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
#include "BDSFieldMagSolenoid.hh"

BDSFieldMagSolenoid::BDSFieldMagSolenoid(G4double aBField):
  itsBField(aBField)
{;}

BDSFieldMagSolenoid::~BDSFieldMagSolenoid()
{;}

void BDSFieldMagSolenoid::GetFieldValue(const G4double* Point[4],
					G4double *Bfield ) const
{
  G4ThreeVector localBField = G4ThreeVector(0.0, 0.0, itsBField);

  G4ThreeVector globalR(Point[0], Point[1], Point[2]);
  auxNavigator->LocateGlobalPointAndSetup(globalR);
  G4AffineTransform localAffine = auxNavigator->GetLocalToGlobalTransform();
  G4ThreeVector globalBField = localAffine.TransformAxis(localBField);

  Bfield[0] = globalBField.x();
  Bfield[1] = globalBField.y();
  Bfield[2] = globalBField.z();

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " B field = "
	 << Bfield[0] << " "
	 << Bfield[1] << " "
	 << Bfield[2] << G4endl;
#endif
}
