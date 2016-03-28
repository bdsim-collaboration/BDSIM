#include "BDSFieldEM.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"

#include <utility>

BDSFieldEM::BDSFieldEM()
{;}

void BDSFieldEM::GetFieldValue(const G4double point[4],
			       G4double* field ) const
{
  std::pair<G4ThreeVector, G4ThreeVector> fieldValue;
  fieldValue = GetField(G4ThreeVector(point[0], point[1], point[2]), point[3]);
  field[0] = fieldValue.first[0];  // B_x
  field[1] = fieldValue.first[1];  // B_y
  field[2] = fieldValue.first[2];  // B_z
  field[3] = fieldValue.second[0]; // E_x
  field[4] = fieldValue.second[1]; // E_y
  field[5] = fieldValue.second[2]; // E_z
}
