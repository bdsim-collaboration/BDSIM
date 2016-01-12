#include "BDSDebug.hh"
#include "BDSFieldMagSextupole.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

#include <cmath>

BDSFieldMagSextupole::BDSFieldMagSextupole(const BDSMagnetStrength* strength,
					   const G4double           brho)
{
  // B'' = d^2By/dx^2 = Brho * (1/Brho d^2By/dx^2) = Brho * k2
  bDoublePrime     = brho * (*strength)["k2"];
  halfBDoublePrime = bDoublePrime*0.5;
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "B'' = " << bDoublePrime << G4endl;
#endif
}

void BDSFieldMagSextupole::GetFieldValue(const G4double point[4],
					 G4double* field) const
{
  G4ThreeVector localPosition = ConvertToLocal(point);
  
  // B_x = 2*x*y * (B''/2!)
  // B_y = (x^2 - y^2) * (B''/2!)
  // B_z = 0

  G4ThreeVector localField;
  localField[0] = localPosition.x() * localPosition.y() * bDoublePrime;
  localField[1] = (pow(localPosition.x(),2) - pow(localPosition.y(),2)) * halfBDoublePrime;
  localField[2] = 0;

  OutputToGlobal(localField, field);
}



