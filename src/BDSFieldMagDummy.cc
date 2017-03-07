#include "BDSFieldMagDummy.hh"

#include "globals.hh" // geant4 types / globals

BDSFieldMagDummy::BDSFieldMagDummy()
{;}

void BDSFieldMagDummy::GetFieldValue(const G4double /*point*/[4],
				     G4double fieldArray[6]) const
{
  for (unsigned int i = 0; i < 6; i++)
    {fieldArray[i] = 0;}
}
