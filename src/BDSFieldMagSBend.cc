#include "BDSDebug.hh"
#include "BDSFieldMagSBend.hh"
#include "BDSMagnetStrength.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"


BDSFieldMagSBend::BDSFieldMagSBend(BDSMagnetStrength const* strength,
				   G4double          const  /*brho*/,
				   G4ThreeVector            unitDirection)
{
  localField  = unitDirection.unit() * (*strength)["field"];
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "B (local) = " << localField << G4endl;
#endif
}

G4ThreeVector BDSFieldMagSBend::GetField(const G4ThreeVector & /*position*/) const
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "Field: " << localField << G4endl;
#endif
  return localField;
}
