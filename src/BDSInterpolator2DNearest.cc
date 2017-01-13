#include "BDSArray2DCoords.hh"
#include "BDSFieldValue.hh"
#include "BDSInterpolator2DNearest.hh"


BDSInterpolator2DNearest::BDSInterpolator2DNearest(BDSArray2DCoords* arrayIn):
  BDSInterpolator2D(arrayIn)
{;}

BDSInterpolator2DNearest::~BDSInterpolator2DNearest()
{;}

BDSFieldValue BDSInterpolator2DNearest::GetInterpolatedValueT(G4double x,
							      G4double y) const
{
  G4int xind = array->NearestX(x);
  G4int yind = array->NearestY(y);
  BDSFieldValue result = array->GetConst(xind, yind); // here we're constructing a copy on purpose
  return result;
}
