#include "BDSDebug.hh"
#include "BDSFieldMagSolenoid.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"


BDSFieldMagSolenoid::BDSFieldMagSolenoid(BDSMagnetStrength* strength,
					 G4double           brho)
{
  localField = G4ThreeVector(0,0,brho * (*strength)["ks"]);
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "B (local) = " << localBField << G4endl;
#endif
}

void BDSFieldMagSolenoid::GetFieldValue(const G4double point[4],
					G4double* field) const
{
  ConvertToLocal(point);

  OutputToGlobal(localField, field);
}
