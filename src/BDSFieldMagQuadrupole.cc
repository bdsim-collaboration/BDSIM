#include "BDSDebug.hh"
#include "BDSFieldMagQuadrupole.hh"
#include "BDSMagnetStrength.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"


BDSFieldMagQuadrupole::BDSFieldMagQuadrupole(const BDSMagnetStrength* strength,
					     const G4double           brho)
{
  // B' = dBy/dx = Brho * (1/Brho dBy/dx) = Brho * k1
  bPrime = brho * (*strength)["k1"];
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "B' = " << bPrime << G4endl;
#endif
}

void BDSFieldMagQuadrupole::GetFieldValue(const G4double point[4],
					  G4double* field) const
{
  G4ThreeVector localPosition = ConvertToLocal(point);
  
  G4ThreeVector localField;
  localField[0] = localPosition.y() * bPrime; // B_x = B' * y;
  localField[1] = localPosition.x() * bPrime; // B_y = B' * x;
  localField[2] = 0;                          // B_z = 0

  OutputToGlobal(localField, field);
}



