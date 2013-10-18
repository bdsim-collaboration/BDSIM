/* BDSIM code.

*/

#include "G4MagneticField.hh"
#include "G4CachedMagneticField.hh"
#include "globals.hh"
#include "G4RotationMatrix.hh"
#include "G4VPhysicalVolume.hh"

#ifndef BDSMagField_h
#define BDSMagField_h

class BDSMagField : public G4MagneticField
{
public:

  // mandatory members

  BDSMagField();
  
  ~BDSMagField();

  virtual G4bool   DoesFieldChangeEnergy() const;

  virtual void GetFieldValue(const G4double Point[4],G4double *Bfield ) const;

  // create a field mesh in the global coordinates after the placement is known
  virtual void Prepare(G4VPhysicalVolume *referenceVolume);

  // aux members

  void SetOriginRotation(G4RotationMatrix rot);
  void SetOriginTranslation(G4ThreeVector trans);

public:
  virtual G4bool GetHasNPoleFields();
  virtual G4bool GetHasUniformField();
  virtual G4bool GetHasFieldMap();

  G4RotationMatrix Rotation() const;
  G4ThreeVector translation;

private:
  G4RotationMatrix* rotation;
  
  
};


#endif
