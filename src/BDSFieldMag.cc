#include "BDSFieldMag.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"

BDSFieldMag::BDSFieldMag():
  transform(G4Transform3D::Identity)
{;}

BDSFieldMag::BDSFieldMag(G4Transform3D transformIn):
  transform(transformIn.inverse())
{;}

G4ThreeVector BDSFieldMag::GetFieldValueTransformed(const G4ThreeVector& position) const
{
  if (transform != G4Transform3D::Identity)
    {
      G4ThreeVector transformedPosition = transform * (HepGeom::Point3D<G4double>)position;
      return GetField(transformedPosition);
    }
  else
    {return GetField(position);}
}

void BDSFieldMag::GetFieldValue(const G4double point[4],
				G4double* field) const
{
  G4ThreeVector fieldValue = GetFieldValueTransformed(G4ThreeVector(point[0], point[1], point[2]));
  field[0] = fieldValue[0]; // B_x
  field[1] = fieldValue[1]; // B_y
  field[2] = fieldValue[2]; // B_z
  field[3] = 0;             // E_x
  field[4] = 0;             // E_y
  field[5] = 0;             // E_z
}
