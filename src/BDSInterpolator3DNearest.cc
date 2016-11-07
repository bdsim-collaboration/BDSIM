#include "BDSArray3DCoords.hh"
#include "BDSFieldValue.hh"
#include "BDSInterpolator3DNearest.hh"


BDSInterpolator3DNearest::BDSInterpolator3DNearest(BDSArray3DCoords* arrayIn):
  BDSInterpolator3D(arrayIn)
{;}

BDSInterpolator3DNearest::~BDSInterpolator3DNearest()
{;}

BDSFieldValue BDSInterpolator3DNearest::GetInterpolatedValueT(G4double x,
							      G4double y,
							      G4double z) const
{
  G4int xind = array->NearestX(x);
  G4int yind = array->NearestY(y);
  G4int zind = array->NearestZ(z);
  BDSFieldValue result = array->GetConst(xind, yind, zind); // here we're constructing a copy on purpose
  return result;
}
