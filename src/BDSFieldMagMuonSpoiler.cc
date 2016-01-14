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

void BDSFieldMagMuonSpoiler::GetFieldValue(const G4double point[4],
					   G4double* field) const
{
  G4ThreeVector localPosition = ConvertToLocal(point);
  
  G4double y = localPosition.x();
  G4double x = localPosition.y();
  G4double r = std::sqrt(pow(x,2) + pow(y,2));

  G4ThreeVector localField;
  localField[0] =  y/r * bField;
  localField[1] = -x/r * bField;
  localField[2] = 0;

  OutputGlobalField(localField, field);
}



