#include "BDSDebug.hh"
#include "BDSFieldMagSextupole.hh"
#include "BDSMagnetStrength.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>

BDSFieldMagSextupole::BDSFieldMagSextupole(BDSMagnetStrength const* strength,
					   G4double          const   brho)
{
  // B'' = d^2By/dx^2 = Brho * (1/Brho d^2By/dx^2) = Brho * k2
  bDoublePrime     = brho * (*strength)["k2"] / CLHEP::m3;
  halfBDoublePrime = bDoublePrime*0.5;
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "B'' = " << bDoublePrime << G4endl;
#endif
}

G4ThreeVector BDSFieldMagSextupole::GetField(const G4ThreeVector &position) const
{ 
  // B_x = 2*x*y * (B''/2!)
  // B_y = (x^2 - y^2) * (B''/2!)
  // B_z = 0

  G4ThreeVector localField;
  localField[0] = position.x() * position.y() * bDoublePrime;
  localField[1] = (pow(position.x(),2) - pow(position.y(),2)) * halfBDoublePrime;
  localField[2] = 0;
  
  return localField;
}
