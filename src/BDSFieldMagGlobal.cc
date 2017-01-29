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

G4ThreeVector BDSFieldMagGlobal::GetField(const G4ThreeVector &position,
					  const G4double       t) const
{
  G4ThreeVector localPosition = ConvertToLocal(position);
  G4ThreeVector localField    = field->GetFieldTransformed(localPosition,t);
  G4ThreeVector globalField   = ConvertAxisToGlobal(localField);

  return globalField;
}
