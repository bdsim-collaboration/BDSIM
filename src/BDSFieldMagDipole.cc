#include "BDSDebug.hh"
#include "BDSFieldMagDipole.hh"
#include "BDSMagnetStrength.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"


BDSFieldMagDipole::BDSFieldMagDipole(BDSMagnetStrength const* strength,
				   G4double          const  /*brho*/,
				   G4ThreeVector            unitDirection)
{
  localField  = unitDirection.unit() * (*strength)["field"];
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "B (local) = " << localField << G4endl;
#endif
}

G4ThreeVector BDSFieldMagDipole::GetField(const G4ThreeVector& /*position*/,
					 const G4double       /*t*/) const
{
  return localField;
}
