#include "BDSDebug.hh"
#include "BDSFieldMagBase.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

BDSFieldMagBase::BDSFieldMagBase()
{;}

void BDSFieldMagBase::OutputGlobalField(const G4ThreeVector& localField,
					G4double* field) const
{
  G4ThreeVector globalField = ConvertAxisToGlobal(localField);

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
