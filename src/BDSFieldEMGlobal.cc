#include "BDSDebug.hh"
#include "BDSFieldEM.hh"
#include "BDSFieldEMGlobal.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

BDSFieldEMGlobal::BDSFieldEMGlobal(BDSFieldEM* fieldIn):
  field(fieldIn)
{;}

BDSFieldEMGlobal::~BDSFieldEMGlobal()
{
  delete field;
}

std::pair<G4ThreeVector,G4ThreeVector> BDSFieldEMGlobal::GetField(const G4ThreeVector& position,
								  const G4double&      t) const
{
  G4ThreeVector localPosition = ConvertToLocal(position);
  auto          localField    = field->GetFieldTransformed(localPosition, t);
  auto          globalField   = ConvertAxisToGlobal(localField);
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "Local Position: " << localPosition << G4endl;
  G4cout << __METHOD_NAME__ << "Local Field:    " << localField    << G4endl;
  G4cout << __METHOD_NAME__ << "Global Field:   " << globalField   << G4endl;
#endif
  return globalField;
}
