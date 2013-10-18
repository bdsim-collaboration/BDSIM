/* BDSIM code.

*/

#include "G4ElectroMagneticField.hh"
#include "globals.hh"
#include "G4RotationMatrix.hh"
#include "G4VPhysicalVolume.hh"

#ifndef BDSField_h
#define BDSField_h

class BDSField : public G4ElectroMagneticField
{
public:

  // mandatory members

  BDSField();
  
  ~BDSField();

  virtual G4bool   DoesFieldChangeEnergy() const;

  virtual void GetFieldValue(const G4double Point[4],G4double *Bfield ) const;

  // create a field mesh in the global coordinates after the placement is known
  virtual void Prepare(G4VPhysicalVolume *referenceVolume);

  // aux members

  void SetOriginRotation(G4RotationMatrix rot);
  void SetOriginTranslation(G4ThreeVector trans);

public:

  G4ThreeVector translation;
  G4RotationMatrix Rotation() const;

private:

  G4RotationMatrix* rotation;

  
};


#endif
