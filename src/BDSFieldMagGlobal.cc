#include "BDSDebug.hh"
#include "BDSField.hh"
#include "BDSFieldMagGlobal.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

BDSFieldMagGlobal::BDSFieldMagGlobal(BDSField* fieldObjectIn, G4ThreeVector localOffsetIn):
  fieldObject(fieldObjectIn), localOffset(localOffsetIn)
{;}

void BDSFieldMagGlobal::GetFieldValue(const G4double point[4],
				      G4double* field) const
{
  G4ThreeVector localPosition = ConvertToLocal(point);
  localPosition += localOffset;
  G4ThreeVector localField    = fieldObject->GetFieldValue(localPosition);
  G4ThreeVector globalField   = ConvertAxisToGlobal(localField);

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "Local B Field:  " << localField  << G4endl;
  G4cout << __METHOD_NAME__ << "Global B Field: " << globalField << G4endl;
#endif
  
  // Electric fields always 0 for fields that inherit this magnetic field
  field[0] = globalField.x();
  field[1] = globalField.y();
  field[2] = globalField.z();
  field[3] = 0;               // E_x = 0
  field[4] = 0;               // E_y = 0
  field[5] = 0;               // E_z = 0
}
