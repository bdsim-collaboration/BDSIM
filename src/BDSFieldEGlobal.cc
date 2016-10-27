#include "BDSDebug.hh"
#include "BDSFieldE.hh"
#include "BDSFieldEGlobal.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

BDSFieldEGlobal::BDSFieldEGlobal(BDSFieldE* fieldIn):
  field(fieldIn)
{;}

BDSFieldEGlobal::~BDSFieldEGlobal()
{
  delete field;
}

G4ThreeVector BDSFieldEGlobal::GetField(const G4ThreeVector& position,
					const G4double       t) const
{
  G4ThreeVector localPosition = ConvertToLocal(position);
  G4ThreeVector localField    = field->GetFieldTransformed(localPosition, t);
  G4ThreeVector globalField   = ConvertAxisToGlobal(localField);
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "Local Position: " << localPosition << G4endl;
  G4cout << __METHOD_NAME__ << "Local Field:    " << localField    << G4endl;
  G4cout << __METHOD_NAME__ << "Global Field:   " << globalField   << G4endl;
#endif
  return globalField;
}
