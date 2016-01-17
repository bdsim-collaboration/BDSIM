#include "BDSDebug.hh"
#include "BDSFieldMagMuonSpoiler.hh"
#include "BDSMagnetStrength.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"

#include <cmath>

BDSFieldMagMuonSpoiler::BDSFieldMagMuonSpoiler(BDSMagnetStrength* const strength,
					       const G4double           brho)
{
  bField = brho * (*strength)["field"];
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "B = " << bField << G4endl;
#endif
}

G4ThreeVector BDSFieldMagMuonSpoiler::GetFieldValue(const G4ThreeVector& position) const
{ 
  G4double x = position.x();
  G4double y = position.y();
  G4double r = std::sqrt(std::pow(x,2) + std::pow(y,2));

  G4ThreeVector localField;
  localField[0] =  y/r * bField;
  localField[1] = -x/r * bField;
  localField[2] = 0;

  return localField;
}



