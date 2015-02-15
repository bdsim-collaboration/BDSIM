// This class returns the B field of a solenoid in GLOBAL coordinates

#include "BDSGlobalConstants.hh"
#include "BDSDebug.hh"

#include "BDSSolenoidMagField.hh"

#include "G4Navigator.hh"
#include "G4TransportationManager.hh"

BDSSolenoidMagField::BDSSolenoidMagField(G4double aBField):itsBField(aBField){
  G4cout<<"Using Geant4 transportation"<<G4endl;
}
BDSSolenoidMagField::~BDSSolenoidMagField(){}

void BDSSolenoidMagField::GetFieldValue( const G4double* /*Point[4]*/,
				      G4double *Bfield ) const
{
  G4Navigator* Navigator=
    G4TransportationManager::GetTransportationManager()->
    GetNavigatorForTracking();
 
  G4ThreeVector LocalR, GlobalR, LocalBField;

  // commented out because solenoid field (uniform) does not depend on position
  //GlobalR.setX(Point[0]);
  //GlobalR.setY(Point[1]);
  //GlobalR.setZ(Point[2]);
  //G4AffineTransform GlobalAffine = Navigator->GetGlobalToLocalTransform();
  //LocalR = GlobalAffine.TransformPoint(GlobalR); 

  LocalBField = G4ThreeVector(0.0, 0.0, itsBField);
  
  G4AffineTransform LocalAffine = Navigator->GetLocalToGlobalTransform();
  G4ThreeVector GlobalBField = LocalAffine.TransformAxis(LocalBField);

  Bfield[0]=GlobalBField.x();
  Bfield[1]=GlobalBField.y();
  Bfield[2]=GlobalBField.z();

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " B field = " << Bfield[0] << " " << Bfield[1] << " " << Bfield[2] << G4endl;
#endif
  
}
