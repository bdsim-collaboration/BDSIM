#ifndef BDSFIELDMAGOUTERMULTIPOLE_H
#define BDSFIELDMAGOUTERMULTIPOLE_H

#include "BDSFieldMag.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

class BDSFieldMagOuterMultipole: public BDSFieldMag
{
public:
  BDSFieldMagOuterMultipole(const G4int    nPolesIn,
			    const G4double polField,
			    const G4double phiOffset);

  virtual ~BDSFieldMagOuterMultipole(){;}

  /// Access the field value.
  virtual G4ThreeVector GetFieldValue(const G4ThreeVector& position) const;

  /// Get the name of the field.
  virtual inline G4String Name() const;

private:
  const G4int    nPoles;
  const G4double fieldStrength;
  const G4double phiOffset;
  G4double itsSectorPhi;
};

inline G4String BDSFieldMagOuterMultipole::Name() const
{return "outer mulitpole";}

#endif
