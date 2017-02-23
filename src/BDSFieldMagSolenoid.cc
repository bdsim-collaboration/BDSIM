#include "BDSDebug.hh"
#include "BDSFieldMagSolenoid.hh"
#include "BDSMagnetStrength.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"


BDSFieldMagSolenoid::BDSFieldMagSolenoid(BDSMagnetStrength const* strength,
					 G4double          const  brho)
{
  localField = G4ThreeVector(0,0,brho * (*strength)["ks"] / CLHEP::m);
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "B (local) = " << localField << G4endl;
#endif
}

G4ThreeVector BDSFieldMagSolenoid::GetField(const G4ThreeVector& /*position*/,
					    const G4double       /*t*/) const
{
  return localField;
}
