#include "BDSDebug.hh"
#include "BDSFieldMagQuadrupole.hh"
#include "BDSMagnetStrength.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

#include "CLHEP/Units/SystemOfUnits.h"

BDSFieldMagQuadrupole::BDSFieldMagQuadrupole(BDSMagnetStrength const* strength,
					     G4double          const  brho)
{
  // B' = dBy/dx = Brho * (1/Brho dBy/dx) = Brho * k1
  bPrime = brho * (*strength)["k1"] / CLHEP::m2;
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "B' = " << bPrime << G4endl;
#endif
}

G4ThreeVector BDSFieldMagQuadrupole::GetField(const G4ThreeVector &position,
					      const G4double       /*t*/) const
{  
  G4ThreeVector field;
  field[0] = position.y() * bPrime; // B_x = B' * y;
  field[1] = position.x() * bPrime; // B_y = B' * x;
  field[2] = 0;                     // B_z = 0

  return field;
}
