#include "BDSInterpolator1D.hh"

#include "globals.hh"

BDSInterpolator1D::BDSInterpolator1D(BDSArray1DCoords* arrayIn):
  array(arrayIn)
{
  if (!array)
    {G4cerr << "Invalid array to construct interpolator on" << G4endl; exit(1);}
}

G4ThreeVector BDSInterpolator1D::GetInterpolatedValue(G4double x) const
{
  BDSFieldValue r = GetInterpolatedValueT(x);
  return G4ThreeVector(r.x(), r.y(), r.z());
}

G4ThreeVector BDSInterpolator1D::GetInterpolatedValue(G4ThreeVector position) const
{
  BDSFieldValue r = GetInterpolatedValueT(position.x());
  return G4ThreeVector(r.x(), r.y(), r.z());
}
