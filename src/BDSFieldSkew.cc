#include "BDSFieldSkew.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4RotationMatrix.hh"

BDSFieldSkew::BDSFieldSkew(BDSField* fieldIn,
			   G4double  angle):
  field(fieldIn),
  rotation(new G4RotationMatrix())
{
  if (BDS::IsFinite(angle))
    {rotation->rotateZ(angle);}
}

BDSFieldSkew::BDSFieldSkew(BDSField*         fieldIn,
			   G4RotationMatrix* rotationIn):
  field(fieldIn),
  rotation(rotationIn)
{;}

BDSFieldSkew::~BDSFieldSkew()
{
  delete field;
  delete rotation;
}

G4ThreeVector BDSFieldSkew::GetFieldValue(const G4ThreeVector& position) const
{
  G4ThreeVector rotatedPosition(position);
  rotatedPosition.transform(*rotation);
  return field->GetFieldValue(rotatedPosition);
}
