#include "BDSUniformMagField.hh"
BDSUniformMagField::~BDSUniformMagField()
{;}

BDSUniformMagField::BDSUniformMagField(const G4ThreeVector& fieldVector):G4UniformMagField(fieldVector){
}

BDSUniformMagField::BDSUniformMagField(G4double vField, G4double vTheta, G4double vPhi)
  :G4UniformMagField(vField, vTheta, vPhi)
{;}

void BDSUniformMagField::Prepare(G4VPhysicalVolume* referenceVolume){
}
