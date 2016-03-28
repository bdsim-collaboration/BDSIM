#include "BDSFieldE.hh"

BDSFieldE::BDSFieldE()
{;}

void BDSFieldE::GetFieldValue(const G4double point[4],
			      G4double* field) const
{
  G4ThreeVector fieldValue = GetField(G4ThreeVector(point[0], point[1], point[2]));
  field[0] = 0;             // B_x
  field[1] = 0;             // B_y
  field[2] = 0;             // B_z
  field[3] = fieldValue[0]; // E_x
  field[4] = fieldValue[1]; // E_y
  field[5] = fieldValue[2]; // E_z
}
