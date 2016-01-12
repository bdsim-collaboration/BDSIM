#include "BDSDebug.hh"
#include "BDSFieldMagBase.hh"

#include "G4AffineTransform.hh"
#include "G4ThreeVector.hh"

BDSFieldMagBase::BDSFieldMagBase()
{;}

G4ThreeVector BDSFieldMagBase::ConvertToLocal(const G4dobule (&point)[4]) const
{
  G4ThreeVector globalPosition(point[0], point[1], point[2]);

  if (!initialised)
    {InitialiseTransform(globalPosition);}

  return globalToLocal.TransformPoint(globalPosition);
}

void BDSFieldMagBase::OutputGlobalField(const G4ThreeVector& localField,
					G4double* field) const
{
  G4ThreeVector globalField = localToGlobal.TransformAxis(localField);

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
