/* BDSIM code.

*/

#include "G4MagneticField.hh"
#include "G4UniformMagField.hh"
#include "G4CachedMagneticField.hh"
#include "globals.hh"
#include "G4RotationMatrix.hh"
#include "G4VPhysicalVolume.hh"
#include "G4UniformMagField.hh"
#include "G4ThreeVector.hh"
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

  //Ovverride the GetFieldValue
  virtual void GetFieldValue(const G4double yTrack[4], G4double *MagField) const;
  virtual void Prepare(G4VPhysicalVolume *referenceVolume);

  G4ThreeVector pmin() const;

  //Set the 'lower left'=pmin and 'upper right'pmax corners of the box containing the field, in local 
  //coordinates.
  virtual void SetFieldExtent(const G4ThreeVector& pmin, const G4ThreeVector& pmax);
  
private:
  void Defaults();

  G4ThreeVector _pmin;
  G4ThreeVector _pmax;
};


#endif
