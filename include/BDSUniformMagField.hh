/* BDSIM code.

*/

#include "G4MagneticField.hh"
#include "G4UniformMagField.hh"
#include "G4CachedMagneticField.hh"
#include "globals.hh"
#include "G4RotationMatrix.hh"
#include "G4VPhysicalVolume.hh"
#include "G4UniformMagField.hh"
#include "BDSMagField.hh"

#ifndef BDSUniformMagField_h
#define BDSUniformMagField_h

class BDSUniformMagField : public BDSMagField, public G4UniformMagField
{
public:
  BDSUniformMagField(const G4ThreeVector& fieldVector);
  BDSUniformMagField(G4double vField, G4double vTheta, G4double vPhi);
  // mandatory members
  ~BDSUniformMagField();

  // create a field mesh in the global coordinates after the placement is known
  virtual void Prepare(G4VPhysicalVolume *referenceVolume);
};


#endif
