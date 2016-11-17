#include "BDSInterpolator3D.hh"

#include "globals.hh"

BDSInterpolator3D::BDSInterpolator3D(BDSArray3DCoords* arrayIn):
  array(arrayIn)
{
  if (!array)
    {G4cerr << "Invalid array to construct interpolator on" << G4endl; exit(1);}
}

G4ThreeVector BDSInterpolator3D::GetInterpolatedValue(G4double x, G4double y, G4double z) const
{
  BDSFieldValue r = GetInterpolatedValueT(x,y,z);
  return G4ThreeVector(r.x(), r.y(), r.z());
}

G4ThreeVector BDSInterpolator3D::GetInterpolatedValue(G4ThreeVector position) const
{
  BDSFieldValue r = GetInterpolatedValueT(position.x(), position.y(), position.z());
  return G4ThreeVector(r.x(), r.y(), r.z());
}
