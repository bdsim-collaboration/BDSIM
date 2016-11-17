#include "BDSFieldE.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"

BDSFieldE::BDSFieldE():
  transform(G4Transform3D::Identity)
{;}

BDSFieldE::BDSFieldE(G4Transform3D transformIn):
  transform(transformIn.inverse())
{;}

G4ThreeVector BDSFieldE::GetFieldTransformed(const G4ThreeVector& position,
					     const G4double       t) const
{
  if (transform != G4Transform3D::Identity)
    {
      G4ThreeVector transformedPosition = transform * (HepGeom::Point3D<G4double>)position;
      return GetField(transformedPosition, t);
    }
  else
    {return GetField(position,t);}
}

void BDSFieldE::GetFieldValue(const G4double point[4],
			      G4double* field) const
{
  G4ThreeVector fieldValue = GetField(G4ThreeVector(point[0], point[1], point[2]), point[3]);
  field[0] = 0;             // B_x
  field[1] = 0;             // B_y
  field[2] = 0;             // B_z
  field[3] = fieldValue[0]; // E_x
  field[4] = fieldValue[1]; // E_y
  field[5] = fieldValue[2]; // E_z
}
