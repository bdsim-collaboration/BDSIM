#include "BDSInterpolator2D.hh"

#include "globals.hh"

BDSInterpolator2D::BDSInterpolator2D(BDSArray2DCoords* arrayIn):
  array(arrayIn)
{
  if (!array)
    {G4cerr << "Invalid array to construct interpolator on" << G4endl; exit(1);}
}

G4ThreeVector BDSInterpolator2D::GetInterpolatedValue(G4double x, G4double y) const
{
  BDSFieldValue r = GetInterpolatedValueT(x,y);
  return G4ThreeVector(r.x(), r.y(), r.z());
}

G4ThreeVector BDSInterpolator2D::GetInterpolatedValue(G4ThreeVector position) const
{
  BDSFieldValue r = GetInterpolatedValueT(position.x(), position.y());
  return G4ThreeVector(r.x(), r.y(), r.z());
}
