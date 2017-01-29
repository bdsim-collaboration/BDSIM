#include "BDSFieldMagSkew.hh"

#include "globals.hh"
#include "G4RotationMatrix.hh"

BDSFieldMagSkew::BDSFieldMagSkew(BDSFieldMag* fieldIn,
				 G4double     angle):
  field(fieldIn)
{
  rotation = new G4RotationMatrix();
  antiRotation = new G4RotationMatrix();
  rotation->rotateZ(angle);
  antiRotation->rotateZ(-angle);
}

BDSFieldMagSkew::~BDSFieldMagSkew()
{
  delete field;
  delete rotation;
  delete antiRotation;
}

G4ThreeVector BDSFieldMagSkew::GetField(const G4ThreeVector &position,
					const G4double       t) const
{
  G4ThreeVector rotatedPosition(position);
  rotatedPosition           = rotatedPosition.transform(*rotation);
  G4ThreeVector normalField = field->GetField(rotatedPosition, t);
  return (*antiRotation)*normalField;
}
