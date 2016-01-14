#ifndef BDSFIELDMAGOUTERMULTIPOLE_H
#define BDSFIELDMAGOUTERMULTIPOLE_H

#include "BDSFieldMagBase.hh"

#include "globals.hh" // geant4 types / globals

class BDSFieldMagOuterMultipole: public BDSFieldMagBase
{
public:
  BDSFieldMagOuterMultipole(const G4int    nPolesIn,
			    const G4double polField,
			    const G4double phiOffset);

  ~BDSFieldMagOuterMultipole();

  virtual void GetFieldValue(const G4double point[4],
			     G4double* field ) const;

private:
  const G4int    nPoles;
  const G4double fieldStrength;
  const G4double phiOffset;
  G4double itsSectorPhi;
};

#endif
