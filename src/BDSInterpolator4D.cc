#include "BDSInterpolator4D.hh"

#include "globals.hh"

BDSInterpolator4D::BDSInterpolator4D(BDSArray4DCoords* arrayIn):
  array(arrayIn)
{
  if (!array)
    {G4cerr << "Invalid array to construct interpolator on" << G4endl; exit(1);}
}

G4ThreeVector BDSInterpolator4D::GetInterpolatedValue(G4double x,
						      G4double y,
						      G4double z,
						      G4double t) const
{
  BDSFieldValue r = GetInterpolatedValueT(x,y,z,t);
  return G4ThreeVector(r.x(), r.y(), r.z());
}

G4ThreeVector BDSInterpolator4D::GetInterpolatedValue(G4ThreeVector position,
						      G4double      t) const
{
  BDSFieldValue r = GetInterpolatedValueT(position.x(), position.y(), position.z(), t);
  return G4ThreeVector(r.x(), r.y(), r.z());
}
