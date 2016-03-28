#include "BDSDebug.hh"
#include "BDSFieldMag.hh"
#include "BDSFieldMagGlobal.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

BDSFieldMagGlobal::BDSFieldMagGlobal(BDSFieldMag* fieldIn):
  field(fieldIn)
{;}

BDSFieldMagGlobal::~BDSFieldMagGlobal()
{
  delete field;
}

G4ThreeVector BDSFieldMagGlobal::GetField(const G4ThreeVector &position) const
{
  G4ThreeVector localPosition = ConvertToLocal(position);
  G4ThreeVector localField    = field->GetFieldValueTransformed(localPosition);
  G4ThreeVector globalField   = ConvertAxisToGlobal(localField);
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "Local Position: " << localPosition << G4endl;
  G4cout << __METHOD_NAME__ << "Local Field:    " << localField    << G4endl;
  G4cout << __METHOD_NAME__ << "Global Field:   " << globalField   << G4endl;
#endif
  return globalField;
}
