#include "BDSFieldSkew.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4RotationMatrix.hh"

#include "BDSDebug.hh"

BDSFieldSkew::BDSFieldSkew(BDSField* fieldIn,
			   G4double  angle):
  field(fieldIn)
{
  rotation = new G4RotationMatrix();
  antiRotation = new G4RotationMatrix();
  rotation->rotateZ(angle);
  rotation->rotateZ(-angle);
}

BDSFieldSkew::~BDSFieldSkew()
{
  delete field;
  delete rotation;
  delete antiRotation;
}

G4ThreeVector BDSFieldSkew::GetFieldValue(const G4ThreeVector& position) const
{
  G4ThreeVector rotatedPosition(position);
  rotatedPosition           = rotatedPosition.transform(*rotation);
  G4ThreeVector normalField = field->GetFieldValue(rotatedPosition);
  return (*antiRotation)*normalField;
}
