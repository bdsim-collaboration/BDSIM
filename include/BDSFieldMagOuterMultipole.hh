#ifndef BDSFIELDMAGOUTERMULTIPOLE_H
#define BDSFIELDMAGOUTERMULTIPOLE_H

#include "BDSField.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

class BDSFieldMagOuterMultipole: public BDSField
{
public:
  BDSFieldMagOuterMultipole(const G4int    nPolesIn,
			    const G4double polField,
			    const G4double phiOffset);

  virtual ~BDSFieldMagOuterMultipole(){;}

  virtual G4ThreeVector GetFieldValue(const G4ThreeVector& position) const;

private:
  const G4int    nPoles;
  const G4double fieldStrength;
  const G4double phiOffset;
  G4double itsSectorPhi;
};

#endif
