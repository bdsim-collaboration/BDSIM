//
// This class returns the B field of a solenoid in GLOBAL coordinates
//
#include "BDSGlobalConstants.hh"

#include "BDSSolenoidMagField.hh"

#include "G4Navigator.hh"
#include "G4TransportationManager.hh"

BDSSolenoidMagField::BDSSolenoidMagField(G4double aBField):BDSUniformMagField(G4ThreeVector(0,0,aBField)), itsBField(aBField){
  G4cout<<"Using Geant4 transportation"<<G4endl;
}
BDSSolenoidMagField::~BDSSolenoidMagField(){}

void BDSSolenoidMagField::SetBField(G4double aBField){ 
  itsBField = aBField; 
  SetFieldValue(G4ThreeVector(0,0,itsBField));
}

